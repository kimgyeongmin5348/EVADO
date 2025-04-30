#include "Network.h"
#include "Player.h"



std::unordered_map<long long, CPlayer> g_other_players;
SOCKET ConnectSocket = INVALID_SOCKET;

CPlayer player;
CGameObject object;


long long g_myid = 0;

void ProcessPacket(char* ptr)
{

    const unsigned char packet_type = ptr[1];

#ifdef _DEBUG
    std::cout << "[PROCESS] 패킷 타입: " << static_cast<int>(packet_type) << std::endl;
#endif

    switch (packet_type)
    {
    case SC_P_USER_INFO: // 클라이언트의 정보를 가지고 있는 패킷 타입
    {
        sc_packet_user_info* packet = reinterpret_cast<sc_packet_user_info*>(ptr);

        // 패킷 크기 검증 추가
        if (packet->size != sizeof(sc_packet_user_info)) {
            std::cerr << "[ERROR] 잘못된 USER_INFO 패킷: "
                << packet->size << " vs " << sizeof(sc_packet_user_info) << std::endl;
            closesocket(ConnectSocket);
            return;
        }

        g_myid = packet->id;
        player.SetPosition(packet->position);

        if (player.GetCamera()) {
            player.GetCamera()->SetPosition(packet->position);
        }
        break;
    }

    case SC_P_LOGIN_FAIL:  // 로그인 실패 처리
    {
        std::cerr << "로그인 실패: 잘못된 비밀번호입니다." << std::endl;
        closesocket(ConnectSocket);
        exit(1);
        break;
    }

    case SC_P_ENTER: // 새로 들어온 플레이어의 정보를 포함하고 있는 패킷 타입
    {
        sc_packet_enter* packet = reinterpret_cast<sc_packet_enter*>(ptr);

        // o_type 필드 처리 추가
        if (packet->o_type != 0) { // 0=플레이어, 1=NPC 등
            std::cout << "NPC 생성: 타입 " << (int)packet->o_type << std::endl;
            return;
        }

        CPlayer new_player;

        // 해당 플레이어의 위치를 패킷에 포함된 좌표로 설정하는 코드 추가
        new_player.SetPosition(packet->position);

        g_other_players[packet->id] = new_player;
        std::cout << "새 플레이어 접속: " << packet->name << " (ID:" << packet->id << ")" << std::endl;

        break;
    }
    case SC_P_MOVE: // 서버가 클라이언트에게 다른 플레이어의 이동 정보를 보내는 패킷 타입
    {
        sc_packet_move* packet = reinterpret_cast<sc_packet_move*>(ptr);
        
        // 수신 정보 출력
        std::cout << "[클라] 서버로부터 위치 수신 - ID: " << packet->id
            << " (" << packet->position.x << ", " << packet->position.y
            << ", " << packet->position.z << ")\n";

        // 이동 처리
        auto it = g_other_players.find(packet->id);
        if (it != g_other_players.end()) {
            // 부드러운 이동을 위해 속도 기반 업데이트
            XMFLOAT3 target = packet->position;
            XMFLOAT3 current = it->second.GetPosition();
            XMFLOAT3 velocity = { // 이부분은 클라랑 이야기 하면서? 하면 될듯
                (packet->position.x - it->second.GetPosition().x) * 10.0f,
                (packet->position.y - it->second.GetPosition().y) * 10.0f,
                (packet->position.z - it->second.GetPosition().z) * 10.0f
            };
            it->second.SetVelocity(velocity);
        }


        break;
    }

    case SC_P_LEAVE: // 서버가 클라에게 다른 플레이어가 게임을 떠났음을 알려주는 패킷 타입
    {
        sc_packet_leave* packet = reinterpret_cast<sc_packet_leave*>(ptr);

        // 플레이어 제거 및 리소스 정리
        auto it = g_other_players.find(packet->id);
        if (it != g_other_players.end()) {
            it->second.ReleaseShaderVariables(); // 그래픽 리소스 해제
            g_other_players.erase(it);

            // 플레이어 퇴장 알림
            std::cout << "플레이어 퇴장: ID=" << packet->id << std::endl;
        }
        break;
    }
    default:
        printf("알 수 없는 패킷 타입 [%d]\n", ptr[1]);
        closesocket(ConnectSocket);
        exit(1);
    }
}


void process_data(char* net_buf, size_t io_byte)
{

    // 패킷 처리 시작 로그
#ifdef _DEBUG
    std::cout << "[RECV] 수신 데이터 크기: " << io_byte << " bytes" << std::endl;
#endif

    char* ptr = net_buf;
    static size_t in_packet_size = 0;
    static size_t saved_packet_size = 0;
    static char packet_buffer[BUF_SIZE];

    while (0 != io_byte) {
        if (0 == in_packet_size) in_packet_size = ptr[0];
        if (in_packet_size > BUF_SIZE) {
            std::cerr << "[ERROR] 패킷 사이즈 초과: " << in_packet_size
                << "/" << BUF_SIZE << " (Connection Closed)" << std::endl;
            closesocket(ConnectSocket);
            return;
        }
        if (io_byte + saved_packet_size >= in_packet_size) {
            memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
            ProcessPacket(packet_buffer);
            ptr += in_packet_size - saved_packet_size;
            io_byte -= in_packet_size - saved_packet_size;
            in_packet_size = 0;
            saved_packet_size = 0;
        }
        else {
            memcpy(packet_buffer + saved_packet_size, ptr, io_byte);
            saved_packet_size += io_byte;
            io_byte = 0;
        }
    }
}

void send_packet(void* packet)
{
    unsigned char* p = reinterpret_cast<unsigned char*>(packet);

#ifdef _DEBUG
    std::cout << "[SEND] 패킷 타입: " << static_cast<int>(p[1])
        << ", 크기: " << static_cast<int>(p[0]) << std::endl;
#endif

    int iSendResult = send(ConnectSocket, (char*)packet, p[0], 0);
    if (iSendResult == SOCKET_ERROR) {
        std::cerr << "[ERROR] 패킷 전송 실패: " << WSAGetLastError() << " (Type: " << static_cast<int>(p[1]) << ")" << std::endl;
    }
}

void send_position_to_server(const XMFLOAT3& position) {
    cs_packet_move p;
    p.size = sizeof(p);
    p.type = CS_P_MOVE;
    p.position = position;
    send_packet(&p);
}