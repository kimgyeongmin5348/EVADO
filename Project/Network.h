#pragma once
#include "Common.h"
#include "Object.h"
//#include "Player.h"
#include "Scene.h"
#include "OtherPlayer.h" 
#include "Object_Items.h"


// 클라이언트에서 서버로 가야할것 -----------
// 
// 플레이어의 좌표(x,y,z)
// 플레이어의 id(user_name)
// 
// 
// 오브젝트의 좌표(ob_x,ob_y,ob_z)
// 
// 
//  <필요한것>
//		플레이어의 정보 저장공간
//		오브젝트의 정보 저장공간
// 
// ----------------------------------------

class Item;

//enum IO_OPERATION { IO_RECV, IO_SEND, IO_CONNECT };
//
//struct OverlappedEx {
//    WSAOVERLAPPED overlapped;
//    WSABUF wsaBuf;
//    char buffer[BUF_SIZE];
//    IO_OPERATION operation;
//};


extern HANDLE g_hIOCP;
extern SOCKET ConnectSocket;
extern std::unordered_map<long long, OtherPlayer*> g_other_players;
extern long long g_myid;
extern std::string user_name;

extern std::unordered_map<long long, Item*> g_items; // 아이템 ID로 관리
extern std::mutex g_item_mutex;

//extern CScene* g_pScene;
extern ID3D12Device* g_pd3dDevice;
extern ID3D12GraphicsCommandList* g_pd3dCommandList;
extern ID3D12RootSignature* g_pd3dGraphicsRootSignature;
extern void* g_pContext;

extern std::queue<std::vector<char>> g_sendQueue;
extern std::mutex g_sendMutex;
extern std::condition_variable g_sendCV;

extern std::unordered_map<long long, CSpider*> g_monsters;
extern std::mutex g_monster_mutex;


// Monster
void SendHitSpider(long long monsterID);


// Shop
void SendShopBuyRequest(int item_type);
void SendShopSellRequest(int item_type);

// Item
void SendItemMove(long long item_id, XMFLOAT3& position);


void ProcessPacket(char* ptr);
//void process_data(char* net_buf, size_t io_byte); // ???
void send_packet(void* packet);
void send_position_to_server(const XMFLOAT3& position, const XMFLOAT3& look, const XMFLOAT3& right, const uint8_t& animState);
void InitializeNetwork(char serverIP[]);
void CleanupNetwork();

void LoadingDoneToServer();


