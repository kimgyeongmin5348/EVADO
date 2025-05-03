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

<<<<<<< Updated upstream
=======
        // 수신 정보 출력
        std::cout << "[클라] 서버로부터 위치 수신 - ID: " << packet->id << " (" << packet->position.x << ", " << packet->position.y << ", " << packet->position.z << ")\n";

        // 이동 처리
>>>>>>> Stashed changes
        auto it = g_other_players.find(packet->id);
        if (it != g_other_players.end()) {
            it->second.SetPosition(packet->position);

            // 다른 플레이어 위치 업데이트 확인
            std::cout << "[클라] " << packet->id << "번 플레이어 위치 갱신: ("
                << packet->position.x << ", "
                << packet->position.y << ", "
                << packet->position.z << ")\n";
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


// process_data() 함수 개선
void process_data(char* net_buf, size_t io_byte) {
    static vector<char> packet_buffer;
    packet_buffer.insert(packet_buffer.end(), net_buf, net_buf + io_byte);

    while (packet_buffer.size() >= sizeof(unsigned char)) {
        unsigned char packet_size = packet_buffer[0];
        if (packet_buffer.size() < packet_size) break;

        // 패킷 유효성 추가 검증
        if (packet_size < 2 || packet_size > BUF_SIZE) {
            std::cerr << "Invalid packet size: " << (int)packet_size << std::endl;
            closesocket(ConnectSocket);
            return;
        }

        ProcessPacket(packet_buffer.data());
        packet_buffer.erase(packet_buffer.begin(), packet_buffer.begin() + packet_size);
    }
}



void send_packet(void* packet)
{
    unsigned char* p = reinterpret_cast<unsigned char*>(packet);

    std::cout << "[클라] 전송 패킷 내용: ";
    for (int i = 0; i < p[0]; ++i) {
        printf("%02X ", p[i]);
    }
    std::cout << std::endl;

    int iSendResult = send(ConnectSocket, (char*)packet, p[0], 0);
    if (iSendResult == SOCKET_ERROR) {
        std::cerr << "[ERROR] 패킷 전송 실패: " << WSAGetLastError() << " (Type: " << static_cast<int>(p[1]) << ")" << std::endl;
    }
}

<<<<<<< Updated upstream
<<<<<<< Updated upstream
void send_position_to_server(const XMFLOAT3& position) {
=======
void SendPlayerPosition(const XMFLOAT3& position) {
>>>>>>> Stashed changes
=======
void send_position_to_server(const XMFLOAT3& position)
{
>>>>>>> Stashed changes
    cs_packet_move p;
    p.size = sizeof(p);
    p.type = CS_P_MOVE;
    p.position = position;
<<<<<<< Updated upstream
    send_packet(&p);

    // 전송 확인 출력
    std::cout << "[클라] 위치 전송: (" << position.x << ", " << position.y << ", " << position.z << ")\n";

=======

    // 실제 패킷 전송 함수 호출 (구현 필요)
    send_packet(&p);

    // 디버그 출력
    std::cout << "[네트워크] 위치 전송: (" << position.x << ", " << position.y << ", " << position.z << ")\n";
>>>>>>> Stashed changes
}