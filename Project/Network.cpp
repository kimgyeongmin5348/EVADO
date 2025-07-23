#include "stdafx.h"
#include "Network.h"
#include "GameFramework.h"
#include <iostream>

// 클라이언트 부분 (not server)
CScene* g_pScene = nullptr;
ID3D12Device* g_pd3dDevice = nullptr;
ID3D12GraphicsCommandList* g_pd3dCommandList = nullptr;
ID3D12RootSignature* g_pd3dGraphicsRootSignature = nullptr;
void* g_pContext = nullptr;

extern CGameFramework gGameFramework;
CGameObject object;

// 전역 변수 정의
SOCKET ConnectSocket = INVALID_SOCKET;
std::atomic<bool> g_running{ true };
//std::string user_name;
long long g_myid = 0;

std::queue<std::vector<char>> g_sendQueue;
std::mutex g_sendMutex;
std::condition_variable g_sendCV;

WSADATA wsaData;

// 객체 관리 맵
std::unordered_map<long long, OtherPlayer*> g_other_players;
std::mutex g_player_mutex;
std::unordered_map<long long, Item*> g_items;
std::mutex g_item_mutex;


// =================================================================
//           몬스터 렌더링을 위한 몬스터 오브젝트 및 관리 
// =================================================================
struct MonsterObject {
    int64_t monsterID;
    XMFLOAT3 position;
    uint8_t state;

    MonsterObject(int64_t id, XMFLOAT3 pos, uint8_t st)
        : monsterID(id), position(pos), state(st) {
    }

    void SetPosition(const XMFLOAT3& pos) { position = pos; }
    void SetState(uint8_t st) { state = st; }

};

std::unordered_map<int64_t, MonsterObject*> g_monsters;
std::mutex g_monster_mutex;


// 몬스터 생성
void OnMonsterSpawned(int64_t monsterID, const XMFLOAT3& pos, uint8_t state) {
    std::lock_guard<std::mutex> lock(g_monster_mutex);
    if (g_monsters.count(monsterID) == 0) {
        g_monsters[monsterID] = new MonsterObject(monsterID, pos, state);
    }
}
// 몬스터 위치, 상태 업데이트
void UpdateMonsterPosition(int64_t monsterID, const XMFLOAT3& pos, uint8_t state) {
    std::lock_guard<std::mutex> lock(g_monster_mutex);
    auto it = g_monsters.find(monsterID);
    if (it != g_monsters.end()) {
        it->second->SetPosition(pos);
        it->second->SetState(state);
    }
   
}


// =================================================================
//                           상점 관리
// =================================================================

void SendShopBuyRequest(int item_type)
{
    cs_packet_shop_buy pkt{};
    pkt.size = sizeof(pkt);
    pkt.type = CS_P_SHOP_BUY;
    pkt.item_type = item_type;
    send_packet(&pkt);
}

void SendShopSellRequest(int item_type)
{
    cs_packet_shop_sell pkt{};
    pkt.size = sizeof(pkt);
    pkt.type = CS_P_SHOP_SELL;
    pkt.item_type = item_type;
    send_packet(&pkt);
}

// =================================================================
//                          아이템 관리
// =================================================================

void SendItemMove(long long item_id, XMFLOAT3& position)
{
    cs_packet_item_move itm;
    itm.size = sizeof(itm);
    itm.type = CS_P_ITEM_MOVE;
    itm.item_id = item_id;
    itm.position = position;
    send_packet(&itm);
}

// =================================================================
//                      네트워크 코어 로직
// =================================================================

void SendThread() {
    while (g_running) {
        std::vector<char> packet;
        {
            std::unique_lock<std::mutex> lock(g_sendMutex);
            g_sendCV.wait(lock, [] { return !g_sendQueue.empty() || !g_running; });
            if (!g_running) break;
            packet = std::move(g_sendQueue.front());
            g_sendQueue.pop();
        }

        WSABUF wsaBuf = { static_cast<ULONG>(packet.size()), packet.data() };
        WSAOVERLAPPED overlapped = {};
        overlapped.hEvent = WSACreateEvent();

        DWORD sent = 0;
        int ret = WSASend(ConnectSocket, &wsaBuf, 1, &sent, 0, &overlapped, nullptr);

        if (ret == SOCKET_ERROR) {
            if (WSAGetLastError() == WSA_IO_PENDING) {
                DWORD result = WSAWaitForMultipleEvents(1, &overlapped.hEvent, TRUE, 1000, FALSE);
                if (result == WSA_WAIT_FAILED) {
                    std::cerr << "송신 오류: " << WSAGetLastError() << std::endl;
                    break;
                }
                WSAGetOverlappedResult(ConnectSocket, &overlapped, &sent, TRUE, nullptr);
            }
            else {
                std::cerr << "송신 실패: " << WSAGetLastError() << std::endl;
            }
        }
        WSACloseEvent(overlapped.hEvent);
    }
}

void RecvThread() {
    thread_local size_t saved_packet_size = 0;
    thread_local char packet_buffer[BUF_SIZE];

    while (g_running) {
        char buffer[MAX_PACKET_SIZE];
        WSABUF wsaBuf = { MAX_PACKET_SIZE, buffer };
        DWORD flags = 0, recvBytes = 0;
        WSAOVERLAPPED overlapped = {};
        overlapped.hEvent = WSACreateEvent();

        int ret = WSARecv(ConnectSocket, &wsaBuf, 1, &recvBytes, &flags, &overlapped, nullptr);

        if (ret == SOCKET_ERROR) {
            if (WSAGetLastError() == WSA_IO_PENDING) {
                DWORD wait = WSAWaitForMultipleEvents(1, &overlapped.hEvent, TRUE, INFINITE, FALSE);
                if (wait == WSA_WAIT_FAILED || !WSAGetOverlappedResult(ConnectSocket, &overlapped, &recvBytes, FALSE, &flags)) {
                    WSACloseEvent(overlapped.hEvent);
                    break;
                }
            }
            else {
                WSACloseEvent(overlapped.hEvent);
                break;
            }
        }

        WSACloseEvent(overlapped.hEvent);

        if (recvBytes == 0) {
            std::cout << "서버 연결 종료" << std::endl;
            g_running = false;
            break;
        }

        // 패킷 처리
        char* ptr = buffer;
        size_t remaining = recvBytes;
        while (remaining > 0) {
            size_t packet_size = ptr[0];
            if (packet_size == 0) break;

            if (saved_packet_size + remaining >= packet_size) {
                memcpy(packet_buffer + saved_packet_size, ptr, packet_size - saved_packet_size);
                ProcessPacket(packet_buffer);
                ptr += packet_size - saved_packet_size;
                remaining -= packet_size - saved_packet_size;
                saved_packet_size = 0;
            }
            else {
                memcpy(packet_buffer + saved_packet_size, ptr, remaining);
                saved_packet_size += remaining;
                remaining = 0;
            }
        }
    }
}

// =================================================================
//                      유틸리티 함수
// =================================================================

void send_packet(void* packet) {
    unsigned char* p = static_cast<unsigned char*>(packet);
    size_t packet_size = p[0];

    std::vector<char> buf(p, p + packet_size);
    {
        std::lock_guard<std::mutex> lock(g_sendMutex);
        g_sendQueue.push(std::move(buf));
    }
    g_sendCV.notify_one();
}

void InitializeNetwork() {
    
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    char serverIP[16];
    std::cout << "server IP : ";
    std::cin >> serverIP;
   

    ConnectSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);

    // 비동기 연결 설정
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, serverIP, &serverAddr.sin_addr);  //serverIP.c_str()
 

    WSAOVERLAPPED connectOverlapped{};
    connectOverlapped.hEvent = WSACreateEvent();


    if (connect(ConnectSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connect Fail: " << WSAGetLastError() << std::endl;
        closesocket(ConnectSocket);
        WSACleanup();
        exit(1);
    }

    WSACloseEvent(connectOverlapped.hEvent);

    std::thread(RecvThread).detach();
    std::thread(SendThread).detach();

    std::cout << "Sever Connect" << std::endl;


    // 로그인 패킷 전송
    cs_packet_login p{};
    p.size = sizeof(p);
    p.type = CS_P_LOGIN;
    strcpy_s(p.name, sizeof(p.name), user_name.c_str());
    send_packet(&p);
  
    std::cout << "[Client] Login Packet Send : Name=" << p.name << std::endl;

}

void ProcessPacket(char* ptr)
{

    const unsigned char packet_type = ptr[1];

    std::cout << "[Client] Packet - Type : " << (int)packet_type << std::endl;

    switch (packet_type)
    {
    // 서버 : 인벤토리 관련된거 만들게 되면 여기에도 정보 추가 해야함
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
            << "Animation : " << static_cast<int>(packet->animState) << ", HP : " << packet->hp << ", CASH : " << packet->cash
            << std::endl;
        break;
    }
    
    // 랜더링 해야함(시선처리 랜더링 해야함)
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
            << "Animation : " << static_cast<int>(packet->animState) << "HP : " << packet->hp << ", CASH : " << packet->cash
            << std::endl;

        // 씬에 OtherPlayer가 딱 나타난다
        gGameFramework.OnOtherClientConnected();

        break;
    }

    // 랜더링 해야함(시선처리 랜더링 해야함)
    case SC_P_MOVE: // 상대 플레이어 (움직이면) 좌표 받기
    {
        sc_packet_move* packet = reinterpret_cast<sc_packet_move*>(ptr);
        int other_id = packet->id;

        if (other_id == g_myid) break;

        // OtherPlayer의 위치를 반영한다
        if (!gGameFramework.isLoading && !gGameFramework.isStartScene) {
            gGameFramework.UpdateOtherPlayerPosition(0, packet->position);
            gGameFramework.UpdateOtherPlayerAnimation(0, packet->animState);
        }


        std::cout << "[Client] New Player Information Recv "
            << " Position(" << packet->position.x << "," << packet->position.y << "," << packet->position.z << ")"
            << " Look(" << packet->look.x << "," << packet->look.y << "," << packet->look.z << ")"
            << " Right(" << packet->right.x << "," << packet->right.y << "," << packet->right.z << ")"
            << "Animation : " << static_cast<int>(packet->animState)
            << std::endl;

        break;
    }

    case SC_P_LEAVE: // 서버가 클라에게 다른 플레이어가 게임을 떠났음을 알려주는 패킷 타입
    {
        sc_packet_leave* packet = reinterpret_cast<sc_packet_leave*>(ptr);
        int other_id = packet->id;

        std::cout << "[Client] Player Remove: ID=" << other_id << std::endl;

        break;
    }

    // 랜더링 해야함
    case SC_P_ITEM_SPAWN:
    {
        sc_packet_item_spawn* pkt = reinterpret_cast<sc_packet_item_spawn*>(ptr);

        std::cout << "[Client] Item Create - ID: " << pkt->item_id
            << " Postion(" << pkt->position.x << ", "
            << pkt->position.y << ", " << pkt->position.z << ")"
            << " Type: " << pkt->item_type << " Cash: " << pkt->cash << std::endl;

        break;
    }

    // 여긴 일단 흠... 사라지는건 update부분에서 해도 되지 않을까...?
    case SC_P_ITEM_DESPAWN:
    {
        sc_packet_item_despawn* pkt = reinterpret_cast<sc_packet_item_despawn*>(ptr);
        std::cout << "[Client] Item delete - ID: " << pkt->item_id << std::endl;
        break;
    }

    // 랜더링 해야함
    case SC_P_ITEM_MOVE:
    {
        sc_packet_item_move* pkt = reinterpret_cast<sc_packet_item_move*>(ptr);

        std::cout << "[Client] Item Move - ID: " << pkt->item_id
            << " Position(" << pkt->position.x << ", "
            << pkt->position.y << ", " << pkt->position.z << ")"
            << std::endl;

        //gGameFramework.UpdateItemPosition(pkt->item_id, pkt->position);

        break;
    }

    // 랜더링 해야함
    case SC_P_MONSTER_SPAWN:
    {
        sc_packet_monster_spawn* pkt = reinterpret_cast<sc_packet_monster_spawn*>(ptr);
        std::cout << "[Client] Monster Spawn - ID: " << pkt->monsterID
            << " Position(" << pkt->position.x << ", " << pkt->position.z << ")"
            << " State: " << static_cast<int>(pkt->state) << std::endl;

        // 몬스터 생성
        OnMonsterSpawned(pkt->monsterID, pkt->position, pkt->state);

        // 랜더링 예시 -> gGameFramework.OnMonsterSpawned(pkt->monsterID, pkt->position);
        break;
    }
    
    // 랜더링 해야함
    case SC_P_MONSTER_MOVE:
    {
        sc_packet_monster_move* pkt = reinterpret_cast<sc_packet_monster_move*>(ptr);

        std::cout << "[Client] Monster Move - ID: " << pkt->monsterID
            << " New Position(" << pkt->position.x << ", " << pkt->position.z << ")"
            << " State: " << static_cast<int>(pkt->state) << std::endl;

        UpdateMonsterPosition(pkt->monsterID, pkt->position, pkt->state);

        // 랜더링 예시 ->  gGameFramework.UpdateMonsterPosition(pkt->monsterID, pkt->position, pkt->state);
        break;
    }

    case SC_P_SHOP_BUY_ACK:
    {
        auto* pkt = reinterpret_cast<sc_packet_shop_buy_ack*>(ptr);
        if (pkt->success)
        {
            std::cout << "[Shop] 구매 성공! 아이템 타입: " << pkt->item_type
                << " 남은 캐시: " << pkt->left_cash << std::endl;

            //  예시 : gGameFramework.OnItemBought(pkt->item_type, pkt->left_cash);
        }
        else
        {
            std::cout << "[Shop] 구매 실패! (잔액 부족/보유중 등)" << std::endl;
            // UI 띄우면 좋음
        }
        break;
    }

    case SC_P_SHOP_SELL_ACK:
    {
        auto* pkt = reinterpret_cast<sc_packet_shop_sell_ack*>(ptr);
        if (pkt->success)
        {
            std::cout << "[Shop] 판매 성공! 아이템 타입: " << pkt->item_type
                << " 남은 캐시: " << pkt->left_cash << std::endl;

            // 예시 : gGameFramework.OnItemSold(pkt->item_type, pkt->left_cash);
        }
        else
        {
            std::cout << "[Shop] 판매 실패! (보유하지 않은 아이템)" << std::endl;
            // UI 띄우면 좋음
        }
        break;
    }


    default:

        std::cout << "Unknown Packet Type [" << ptr[1] << "]" << std::endl;
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

void CleanupNetwork() {
    g_running = false;
    closesocket(ConnectSocket);
    WSACleanup();
    g_sendCV.notify_all(); // 송신 스레드 깨우기
}

void LoadingDoneToServer()
{
    cs_packet_loading_done pkt{};
    pkt.size = sizeof(pkt);
    pkt.type = CS_P_LOADING_DONE;
    send_packet(&pkt);
    std::cout << "[Client] LodingDone ! " << std::endl;
}