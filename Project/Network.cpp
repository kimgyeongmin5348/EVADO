#include "Network.h"


CScene* g_pScene = nullptr;
ID3D12Device* g_pd3dDevice = nullptr;
ID3D12GraphicsCommandList* g_pd3dCommandList = nullptr;
ID3D12RootSignature* g_pd3dGraphicsRootSignature = nullptr;
void* g_pContext = nullptr;

//std::unordered_map<long long, CPlayer> g_other_players;
std::unordered_map<long long, OtherPlayer*> g_other_players;

std::mutex g_player_mutex; // 멀티스레드 접근 방지


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
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
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

    std::cout << "[클라] 로그인 패킷 전송: 이름=" << p.name << "\n";
}

void ProcessPacket(char* ptr)
{

    const unsigned char packet_type = ptr[1];

    std::cout << "[클라] 패킷 처리 시작 - 타입: " << (int)packet_type << "\n";

    switch (packet_type)
    {
    case SC_P_USER_INFO: // 클라이언트의 정보를 가지고 있는 패킷 타입
    {
        sc_packet_user_info* packet = reinterpret_cast<sc_packet_user_info*>(ptr);

        g_myid = packet->id;
        //player.SetPosition(packet->position);
      

        std::cout << "[클라] 내 플레이어 생성: " << packet->id << std::endl;
        std::cout << "[클라] 내 정보 수신 - ID:" << packet->id
            << " 위치(" << packet->position.x << "," << packet->position.y << "," << packet->position.z << ")"
            << " Look(" << packet->look.x << "," << packet->look.y << "," << packet->look.z << ")"
            << " Right(" << packet->right.x << "," << packet->right.y << "," << packet->right.z << ")"
            << std::endl;
        break;
    }

    case SC_P_ENTER: // 새로 들어온 플레이어의 정보를 포함하고 있는 패킷 타입
    {
        sc_packet_enter* packet = reinterpret_cast<sc_packet_enter*>(ptr);
        int id = packet->id;

        if (id == g_myid) break;
        
        std::cout << "새로운 플레이어" << id << "접속 성공" << "\n";
        
        
        break;
    }
    case SC_P_MOVE: 
    {
        sc_packet_move* packet = reinterpret_cast<sc_packet_move*>(ptr);
        int other_id = packet->id;

        if (other_id == g_myid) break;

        // 다른 플레이어 위치 업데이트 확인
        std::cout << "[클라] " << other_id << "번 플레이어 위치 갱신: ("
            << packet->position.x << ", "
            << packet->position.y << ", "
            << packet->position.z << ") "
            << "Look(" << packet->look.x << ", " << packet->look.y << ", " << packet->look.z << ") "
            << "Right(" << packet->right.x << ", " << packet->right.y << ", " << packet->right.z << ")\n";


        //if (other_id != g_myid || other_id < MAX_USER) { // 다른 플레이어 위치 갱신
        //    // 다른 플레이어 위치 업데이트 확인
        //    std::cout << "[클라] " << other_id << "번 플레이어 위치 갱신: ("
        //        << packet->position.x << ", "
        //        << packet->position.y << ", "
        //        << packet->position.z << ")\n";
        //}


        
        break;
    }

    case SC_P_LEAVE: // 서버가 클라에게 다른 플레이어가 게임을 떠났음을 알려주는 패킷 타입
    {
        sc_packet_leave* packet = reinterpret_cast<sc_packet_leave*>(ptr);
        int other_id = packet->id;

        std::cout << "[클라] 플레이어 제거: ID=" << other_id << "\n";
        

        break;
    }
    case SC_P_ITEM_SPAWN: {
        sc_packet_item_spawn* pkt = reinterpret_cast<sc_packet_item_spawn*>(ptr);
        std::cout << "[클라] 아이템 생성 - ID: " << pkt->item_id
            << " 위치(" << pkt->position.x << ", "
            << pkt->position.y << ", " << pkt->position.z << ")"
            << " 타입: " << pkt->item_type << "\n";
        break;
    }

    case SC_P_ITEM_DESPAWN: {
        sc_packet_item_despawn* pkt = reinterpret_cast<sc_packet_item_despawn*>(ptr);
        std::cout << "[클라] 아이템 삭제 - ID: " << pkt->item_id << "\n";
        break;
    }

    case SC_P_ITEM_MOVE: {
        sc_packet_item_move* pkt = reinterpret_cast<sc_packet_item_move*>(ptr);
        std::cout << "[디버그] 아이템 이동 - "
            << "ID: " << pkt->item_id << ", "
            << "위치: (" << pkt->position.x << ", "
            << pkt->position.y << ", " << pkt->position.z << "), "
            << "소유자: " << (pkt->holder_id == g_myid ? "본인" : "타인")
            << " (" << pkt->holder_id << ")\n";

        //if (pkt->holder_id == g_myid) {
        //    // 자신이 들고 있는 아이템은 별도 처리
        //    player.UpdateHeldItemPosition(pkt->position);
        //}
        //else {
        //    g_pScene->MoveItem(pkt->item_id, pkt->position);
        //}
        //break;
    }
    default:
        printf("알 수 없는 패킷 타입 [%d]\n", ptr[1]);
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

void send_position_to_server(const XMFLOAT3& position, const XMFLOAT3& look, const XMFLOAT3& right)
{

    cs_packet_move p;
    p.size = sizeof(p);
    p.type = CS_P_MOVE;
    p.position = position;
    p.look = look;
    p.right = right;
    

    send_packet(&p);

    // 전송 확인 출력
    std::cout << "[클라] 위치 전송: (" 
        << position.x << ", " << position.y << ", " << position.z << ") "
        << "Look(" << look.x << ", " << look.y << ", " << look.z << ") "
        << "Right(" << right.x << ", " << right.y << ", " << right.z << ")\n";

}