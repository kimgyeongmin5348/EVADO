#pragma once
#include "Common.h"
#include "Object.h"
//#include "Player.h"
#include "Scene.h"
#include "OtherPlayer.h" 
#include "Object_Items.h"


// Ŭ���̾�Ʈ���� ������ �����Ұ� -----------
// 
// �÷��̾��� ��ǥ(x,y,z)
// �÷��̾��� id(user_name)
// 
// 
// ������Ʈ�� ��ǥ(ob_x,ob_y,ob_z)
// 
// 
//  <�ʿ��Ѱ�>
//		�÷��̾��� ���� �������
//		������Ʈ�� ���� �������
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

extern std::unordered_map<long long, Item*> g_items; // ������ ID�� ����
extern std::mutex g_item_mutex;

//extern CScene* g_pScene;
extern ID3D12Device* g_pd3dDevice;
extern ID3D12GraphicsCommandList* g_pd3dCommandList;
extern ID3D12RootSignature* g_pd3dGraphicsRootSignature;
extern void* g_pContext;

extern std::queue<std::vector<char>> g_sendQueue;
extern std::mutex g_sendMutex;
extern std::condition_variable g_sendCV;


// Monster
void OnMonsterSpawned(int64_t monsterID, const XMFLOAT3& pos, uint8_t state);
void UpdateMonsterPosition(int64_t monsterID, const XMFLOAT3& pos, uint8_t state);


// Shop
void SendShopBuyRequest(int item_type);
void SendShopSellRequest(int item_type);



void ProcessPacket(char* ptr);
//void process_data(char* net_buf, size_t io_byte); // ???
void send_packet(void* packet);
void send_position_to_server(const XMFLOAT3& position, const XMFLOAT3& look, const XMFLOAT3& right, const uint8_t& animState);
void InitializeNetwork();
void CleanupNetwork();

void LoadingDoneToServer();

