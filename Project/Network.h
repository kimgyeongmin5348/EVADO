#pragma once
#include "Common.h"
#include "Object.h"
#include "Player.h"

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

constexpr int MAX_PACKET_SIZE = 4096;

enum IO_OPERATION { IO_RECV, IO_SEND, IO_CONNECT };

struct OverlappedEx {
    WSAOVERLAPPED overlapped;
    WSABUF wsaBuf;
    char buffer[MAX_PACKET_SIZE];
    IO_OPERATION operation;
    int bytesTransferred;
};


extern HANDLE g_hIOCP;
extern SOCKET ConnectSocket;
extern std::unordered_map<long long, CPlayer> g_other_players;
extern long long g_myid;
extern std::string user_name;

void ProcessPacket(char* ptr);
void process_data(char* net_buf, size_t io_byte);
void send_packet(void* packet);
void send_position_to_server(const XMFLOAT3& position);
void InitializeNetwork();


