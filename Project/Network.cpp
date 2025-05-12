#include "stdafx.h"
#include "Network.h"
#include "GameFramework.h"

CScene* g_pScene = nullptr;
ID3D12Device* g_pd3dDevice = nullptr;
ID3D12GraphicsCommandList* g_pd3dCommandList = nullptr;
ID3D12RootSignature* g_pd3dGraphicsRootSignature = nullptr;
void* g_pContext = nullptr;

extern CGameFramework gGameFramework;

std::unordered_map<long long, OtherPlayer*> g_other_players;
std::mutex g_player_mutex; // 멀티스레드 접근 방지

std::unordered_map<long long, Item*> g_items;
std::mutex g_item_mutex;

SOCKET ConnectSocket = INVALID_SOCKET;
HANDLE g_hIOCP = INVALID_HANDLE_VALUE;

WSADATA wsaData;

CPlayer player;
CGameObject object;
long long g_myid = 0;

char recv_buffer[MAX_BUFFER];
int  saved_data = 0;

void PostRecv();


DWORD WINAPI WorkerThread(LPVOID lpParam) {
    while (true) {
        DWORD bytesTransferred = 0;
        ULONG_PTR completionKey = 0;
        OverlappedEx* overlapped = nullptr;

        BOOL result = GetQueuedCompletionStatus(g_hIOCP, &bytesTransferred, &completionKey, reinterpret_cast<LPOVERLAPPED*>(&overlapped), INFINITE);

        if (!result || bytesTransferred == 0) {
            int error_code = result ? WSAGetLastError() : WSAECONNRESET;
            std::cerr << "[클라] 연결 종료. 오류 코드: " << error_code << std::endl;

            // 1. 소켓 정리
            if (ConnectSocket != INVALID_SOCKET) {
                closesocket(ConnectSocket);
                ConnectSocket = INVALID_SOCKET;
            }

            // 2. 다른 플레이어 데이터 초기화
            g_other_players.clear();
            g_myid = 0;

            // 3. 서버에 연결 종료 알림 (옵션)
            //PostQuitMessage(0); // GUI 애플리케이션인 경우

            delete overlapped;
            continue;
        }

        switch (overlapped->operation) {
        case IO_RECV:
            if (bytesTransferred > 0) {
                process_data(overlapped->buffer, bytesTransferred);
                PostRecv();
            }
            delete overlapped;
            break;
        case IO_SEND:
            delete overlapped;
            break;
        }
    }
    return 0;
}

void PostRecv() {
    if (ConnectSocket == INVALID_SOCKET) return;
    OverlappedEx* overlapped = new OverlappedEx{};
    overlapped->operation = IO_RECV;
    overlapped->wsaBuf.buf = overlapped->buffer;
    overlapped->wsaBuf.len = MAX_PACKET_SIZE;

    DWORD flags = 0;
    WSARecv(ConnectSocket, &overlapped->wsaBuf, 1, nullptr, &flags, reinterpret_cast<LPWSAOVERLAPPED>(overlapped), nullptr);
}

void send_packet(void* packet) {

    if (ConnectSocket == INVALID_SOCKET) return;
    unsigned char* p = reinterpret_cast<unsigned char*>(packet);
    int packet_size = p[0];

    OverlappedEx* overlapped = new OverlappedEx{};
    overlapped->operation = IO_SEND;
    memcpy(overlapped->buffer, packet, packet_size);

    overlapped->wsaBuf.buf = overlapped->buffer;
    overlapped->wsaBuf.len = packet_size;

    int result = WSASend(ConnectSocket, &overlapped->wsaBuf, 1, nullptr, 0, reinterpret_cast<LPWSAOVERLAPPED>(overlapped), nullptr);

    if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        std::cerr << "WSASend 오류: " << WSAGetLastError() << std::endl;
        delete overlapped;
    }


}

void InitializeNetwork()
{
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // 1. IOCP 핸들 생성
    g_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, NUM_WORKER_THREADS);

    // 2. 워커 스레드 생성
    for (int i = 0; i < NUM_WORKER_THREADS; ++i) {
        CreateThread(NULL, 0, WorkerThread, NULL, 0, NULL);
    }

    // 3. Overlapped 소켓 생성
    ConnectSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

    // 4. 논블로킹 소켓 설정
    u_long nonBlockingMode = 1;
    ioctlsocket(ConnectSocket, FIONBIO, &nonBlockingMode);

    // 5. 비동기 연결 설정
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "192.168.78.208", &serverAddr.sin_addr); // 경민노트북
    serverAddr.sin_port = htons(SERVER_PORT);

    // 6. 비동기 연결 시작
    int connectResult = WSAConnect(ConnectSocket, (sockaddr*)&serverAddr, sizeof(serverAddr), NULL, NULL, NULL, NULL);

    if (connectResult == SOCKET_ERROR) {
        int err = WSAGetLastError();
        if (err != WSAEWOULDBLOCK) {
            std::cerr << "연결 실패: " << err << std::endl;
            closesocket(ConnectSocket);
            WSACleanup();
            exit(1);
        }
    }

    // 7. IOCP에 소켓 등록
    CreateIoCompletionPort((HANDLE)ConnectSocket, g_hIOCP, 0, 0);

    // 8. 초기 수신 작업 시작
    PostRecv();

    std::cout << "서버에 성공적으로 연결되었습니다." << std::endl;

    // 9. 로그인 패킷 전송
    cs_packet_login p;
    p.size = sizeof(p);
    p.type = CS_P_LOGIN;
    // p.position = player.GetPosition();
    strcpy_s(p.name, sizeof(p.name), user_name.c_str());
    send_packet(&p);

    std::cout << "[클라] 로그인 패킷 전송: 이름=" << p.name << std::endl;
}

void ProcessPacket(char* ptr)
{

    const unsigned char packet_type = ptr[1];

    std::cout << "[Client] Packet - Type : " << (int)packet_type << std::endl;

    switch (packet_type)
    {
    case SC_P_USER_INFO: // 클라이언트의 정보를 가지고 있는 패킷 타입
    {
        sc_packet_user_info* packet = reinterpret_cast<sc_packet_user_info*>(ptr);

        g_myid = packet->id;
        //player.SetPosition(packet->position);


        std::cout << "[Client] My Player : " << packet->id << std::endl;
        std::cout << "[Client] My Player Information ID:" << packet->id
            << " Positino(" << packet->position.x << "," << packet->position.y << "," << packet->position.z << ")"
            << " Look(" << packet->look.x << "," << packet->look.y << "," << packet->look.z << ")"
            << " Right(" << packet->right.x << "," << packet->right.y << "," << packet->right.z << ")"
            << "Animation : " << static_cast<int>(packet ->animState)
            << std::endl;
        break;
    }

    case SC_P_ENTER: // 새로 들어온 플레이어의 정보를 포함하고 있는 패킷 타입
    {
        sc_packet_enter* packet = reinterpret_cast<sc_packet_enter*>(ptr);
        int id = packet->id;

        if (id == g_myid) break;

        std::cout << "[Client] New Player " << id << "Connect " << "\n";
        std::cout << "[Client] New Player Information Recv "
            << " Position(" << packet->position.x << "," << packet->position.y << "," << packet->position.z << ")"
            << " Look(" << packet->look.x << "," << packet->look.y << "," << packet->look.z << ")"
            << " Right(" << packet->right.x << "," << packet->right.y << "," << packet->right.z << ")"
            << "Animation : " << static_cast<int>(packet->animState)
            << std::endl;
        
        // 씬에 OtherPlayer가 딱 나타난다
        gGameFramework.OnOtherClientConnected();

        break;
    }

    case SC_P_MOVE: // 상대 플레이어 (움직이면) 좌표 받기
    {
        sc_packet_move* packet = reinterpret_cast<sc_packet_move*>(ptr);
        int other_id = packet->id;

        if (other_id == g_myid) break;

        // OtherPlayer의 위치를 반영한다
        if (!gGameFramework.isLoading && !gGameFramework.isStartScene) {
            packet->position.x += 3.8;
            gGameFramework.UpdateOtherPlayerPosition(0, packet->position);
            gGameFramework.UpdateOtherPlayerAnimation(0, packet->animState);
        }


        //std::cout << "[Client] New Player Information Recv "
        //    << " Position(" << packet->position.x << "," << packet->position.y << "," << packet->position.z << ")"
        //    << " Look(" << packet->look.x << "," << packet->look.y << "," << packet->look.z << ")"
        //    << " Right(" << packet->right.x << "," << packet->right.y << "," << packet->right.z << ")"
        //    << "Animation : " << static_cast<int>(packet->animState)
        //    << std::endl;

        break;
    }

    case SC_P_LEAVE: // 서버가 클라에게 다른 플레이어가 게임을 떠났음을 알려주는 패킷 타입
    {
        sc_packet_leave* packet = reinterpret_cast<sc_packet_leave*>(ptr);
        int other_id = packet->id;

        std::cout << "[Client] 플레이어 제거: ID=" << other_id << std::endl;

        break;
    }

    case SC_P_ITEM_SPAWN: 
    {
        sc_packet_item_spawn* pkt = reinterpret_cast<sc_packet_item_spawn*>(ptr);

        // 에시
        // gGameFramework.AddItemToScene(pkt->item_id, static_cast<ITEM_TYPE>(pkt->item_type), pkt->position);

        std::cout << "[Client] Item Create - ID: " << pkt->item_id
            << " Postion(" << pkt->position.x << ", "
            << pkt->position.y << ", " << pkt->position.z << ")"
            << " Type: " << pkt->item_type << std::endl;

        break;
    }

    case SC_P_ITEM_DESPAWN: 
    {
        sc_packet_item_despawn* pkt = reinterpret_cast<sc_packet_item_despawn*>(ptr);
        std::cout << "[Client] Item delete - ID: " << pkt->item_id << std::endl;
        break;
    }

    case SC_P_ITEM_MOVE: 
    {
        sc_packet_item_move* pkt = reinterpret_cast<sc_packet_item_move*>(ptr);


        break;
    }

    case SC_P_MONSTER_SPAWN:
    {
        sc_packet_monster_spawn* pkt = reinterpret_cast<sc_packet_monster_spawn*>(ptr);
        std::cout << "[Client] Monster Spawn - ID: " << pkt->monsterID
            << " Position(" << pkt->position.x << ", " << pkt->position.z << ")"
            << " State: " << static_cast<int>(pkt->state) << std::endl;

        // 몬스터 생성
        // 예시 -> gGameFramework.OnMonsterSpawned(pkt->monsterID, pkt->position);
        break;
    }

    case SC_P_MONSTER_MOVE:
    {
        sc_packet_monster_move* pkt = reinterpret_cast<sc_packet_monster_move*>(ptr);

        std::cout << "[Client] Monster Move - ID: " << pkt->monsterID
            << " New Position(" << pkt->position.x << ", " << pkt->position.z << ")"
            << " State: " << static_cast<int>(pkt->state) << std::endl;

        //몬스터 위치 업데이트 로직
        // 예시 ->  gGameFramework.UpdateMonsterPosition(pkt->monsterID, pkt->position, pkt->state);
        break;
    }
    

    default:
        std::cout << "알 수 없는 패킷 타입 [" << ptr[1] << "]" << std::endl;
    }
}

// process_data() 함수 개선
void process_data(char* net_buf, size_t io_byte) {

    char* ptr = net_buf;
    static size_t in_packet_size = 0;
    static size_t saved_packet_size = 0;
    static char packet_buffer[BUF_SIZE];

    while (0 != io_byte) {
        if (0 == in_packet_size) in_packet_size = ptr[0];
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

void send_position_to_server(const XMFLOAT3& position, const XMFLOAT3& look, const XMFLOAT3& right, const uint8_t& animState)
{

    cs_packet_move p;
    p.size = sizeof(p);
    p.type = CS_P_MOVE;
    p.position = position;
    p.look = look;
    p.right = right;
    p.animState = animState;
    send_packet(&p);

}