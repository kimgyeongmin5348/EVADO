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




extern SOCKET ConnectSocket;

void ProcessPacket(char* ptr);
void process_data(char* net_buf, size_t io_byte);
void send_packet(void* packet);
<<<<<<< Updated upstream
void send_position_to_server(const XMFLOAT3& position);
=======
void SendPlayerPosition(const DirectX::XMFLOAT3& position);
>>>>>>> Stashed changes

// 패킷 전송 함수 선언
extern void send_position_to_server(const DirectX::XMFLOAT3& position);


//extern OBJECT monster, user;
//std::vector<OBJECT> players[MAX_USER];
extern std::unordered_map<long long, CPlayer> g_other_players;
extern long long g_myid;
extern std::string user_name;