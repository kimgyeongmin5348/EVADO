#include "Network.h"
#include "Player.h"


CScene* g_pScene = nullptr;
ID3D12Device* g_pd3dDevice = nullptr;
ID3D12GraphicsCommandList* g_pd3dCommandList = nullptr;
ID3D12RootSignature* g_pd3dGraphicsRootSignature = nullptr;
void* g_pContext = nullptr;

std::unordered_map<long long, CPlayer> g_other_players;
SOCKET ConnectSocket = INVALID_SOCKET;
HANDLE g_hIOCP = INVALID_HANDLE_VALUE;

CPlayer player;
CGameObject object;
long long g_myid = 0;

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
    WSADATA wsaData;
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
    p.position = player.GetPosition();
    strcpy_s(p.name, sizeof(p.name), user_name.c_str());
    send_packet(&p);

    std::cout << "[클라] 로그인 패킷 전송: 이름=" << p.name << " 위치(" << p.position.x << "," << p.position.y << "," << p.position.z << ")\n";
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

        std::cout << "[클라] 내 정보 수신 - ID:" << packet->id << " 위치(" << packet->position.x << "," << packet->position.y << "," << packet->position.z << ")\n";

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
        std::cout << "[클라] 새 플레이어 생성: " << packet->id << std::endl;
        g_pScene->AddRemotePlayer(packet->id, packet->position, g_pd3dDevice, g_pd3dCommandList, g_pd3dGraphicsRootSignature, g_pContext);

        //// 새 플레이어 객체 생성 및 초기화
        //if (g_other_players.find(packet->id) == g_other_players.end()) {
        //    g_other_players[packet->id] = CPlayer{};
        //    std::cout << "[클라] 새 플레이어 생성: " << packet->id << std::endl;
        //}
        //g_other_players[packet->id].SetPosition(packet->position);
        break;
    }
    case SC_P_MOVE: // 서버가 클라이언트에게 다른 플레이어의 이동 정보를 보내는 패킷 타입
    {
        sc_packet_move* packet = reinterpret_cast<sc_packet_move*>(ptr);
        std::cout << "[클라] 이동 패킷 수신 - ID: " << packet->id << " (" << packet->position.x << ", " << packet->position.y << ", " << packet->position.z << ")\n";
        g_pScene->UpdateRemotePlayer(packet->id, packet->position);



        // 이동 처리

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

    while (!packet_buffer.empty()) {
        // 1. 최소 패킷 크기 검증
        if (packet_buffer.size() < sizeof(unsigned char)) break;
        unsigned char packet_size = packet_buffer[0];

        // 2. 패킷 크기 유효성 검사
        if (packet_size < 2 || packet_size > MAX_PACKET_SIZE) {
            std::cerr << "[클라] 잘못된 패킷 크기: " << (int)packet_size << std::endl;
            closesocket(ConnectSocket);
            packet_buffer.clear();
            return;
        }

        // 3. 완전한 패킷 도착 확인
        if (packet_buffer.size() < packet_size) break;

        // 4. 패킷 처리
        ProcessPacket(packet_buffer.data());

        // 5. 처리된 데이터 제거
        packet_buffer.erase(packet_buffer.begin(), packet_buffer.begin() + packet_size);
    }
}


void send_position_to_server(const XMFLOAT3& position)
{

    cs_packet_move p;
    p.size = sizeof(p);
    p.type = CS_P_MOVE;
    p.position = position;

    send_packet(&p);

    // 전송 확인 출력
    std::cout << "[클라] 위치 전송: (" << position.x << ", " << position.y << ", " << position.z << ")\n";

}