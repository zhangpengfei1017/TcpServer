
#include "Header.h"

std::vector<Room*> AllRooms;//所有房间
std::vector<Player*> PlayersInHall;//在大厅中的玩家
std::vector<Player*> AllPlayers;
int NextRoomID = 1;
std::ofstream outfile("output.txt", std::ios::in | std::ios::trunc);


struct HeadPackage_C00 {
	int UID;
	int CmdID;
};


struct JoinRoom_C03 {
	int RoomID;
};
struct PlayerInfo_C08 {
	int posx;
	int posy;
	int posz;
};



//
struct HeadPackage_S00 {
	int UID;
	int CmdID;
};

struct HallInfo_S01 {
	int RoomNum;
};
struct RoomInHall_S01 {
	int RoomId;
	int PlayerNum;
	int P1UID;
};
struct RoomInfo_S02 {
	int RoomId;
};

struct RoomCurentInfo_S05 {
	int RoomID;
	int UIDs[4];
	int PlayerNum;
};
struct GameStartInfoHead_S06 {
	int playernum;
};
struct GameStartPlayerInfo_S06 {
	int UID;
};
struct GameStartDungeonInfo_S06 {
	int maptype;
	int monnum;
};
struct GameStartMonsterInfo_S06 {
	int MonID;
	int type;
	int PosX;
	int PosY;
	int PosZ;
	int maxHp;
	int curHp;
	int attack;
};
struct GameInfoHead_S08 {
	int playernum;
};
struct GamePlayerInfo_S08 {
	int UID;
	int posx;
	int posy;
	int posz;
	int maxhp;
	int curhp;
	int maxmp;
	int curmp;
};
struct GameMonsterInfo_S08 {
	int ID;
	int posx;
	int posy;
	int posz;
	int maxhp;
	int curhp;
	int maxmp;
	int curmp;
};

void ReceiveMsg(char buf[]) {

	//解构客户端命令并调用特定函数处理命令
	int BufPos = 0;
	HeadPackage_C00 hp;
	memset(&hp, 0, sizeof(hp));
	memcpy(&hp, buf, sizeof(hp));
	BufPos += sizeof(hp);
	if (hp.CmdID != 1 && hp.CmdID != 8) {
		std::cout << "从" << hp.UID << "接收了" << hp.CmdID << std::endl;
	}
	if (FindInAll(hp.UID) == nullptr) {
		return;
	}
	switch (hp.CmdID)
	{
	case 0:
	case 1://heart beat test .
		FindInAll(hp.UID)->LastConnectTime = Time::NowTime();
		break;
	case 2://create a new room .
		NewRoom(hp.UID);
		break;
	case 3://join a existing room .
		JoinRoom_C03 jr;
		memset(&jr, 0, sizeof(jr));
		memcpy(&jr, buf + BufPos, sizeof(jr));
		BufPos += sizeof(jr);
		JoinRoom(jr.RoomID, hp.UID);
		break;
	case 4://exit the hall// .
		PlayerExit(hp.UID);
		break;
	case 5://exit the room .
		PlayerExitRoom(hp.UID);
		break;
	case 6: {//start the game ?
		Room* r = FindRoom(FindInAll(hp.UID)->CurRoom);
		int num = r->PlayerNum;
		if (hp.UID == r->Players[0]->UID) {
			r->mDungeon->NewDungeon(num);
			for (int i = 0; i < num; i++) {
				SendMsg(FindRoom(FindInAll(hp.UID)->CurRoom)->Players[i], 6, FindInAll(hp.UID)->CurRoom, nullptr);
			}
		}		
		break;
	}
	case 7: {
		Room* r = FindRoom(FindInAll(hp.UID)->CurRoom);
		r->ReadyNum++;
		if (r->ReadyNum == r->PlayerNum) {
			r->IsRoom = false;
			for (int i = 0; i < r->PlayerNum; i++) {
				SendMsg(r->Players[i], 7, 0, nullptr);
			}
		}
		break; }
	case 8: {
		PlayerInfo_C08 pi;
		memset(&pi, 0, sizeof(pi));
		memcpy(&pi, buf + BufPos, sizeof(pi));
		BufPos += sizeof(pi);
		Player* p = FindInAll(hp.UID);
		p->curPosX = pi.posx;
		p->curPosY = pi.posy;
		p->curPosZ = pi.posz;
	}
	default:
		break;
	}
}
void SendMsg(Player* p, int cmdId, int MsgNum, char MsgChar[]) {
	HeadPackage_S00 hp = { p->UID,cmdId };
	char BufSend[10240];
	int BufPos = 0;
	memset(BufSend, 0, 10240);
	memcpy(BufSend, &hp, sizeof(hp));
	BufPos += sizeof(hp);
	switch (cmdId)
	{
	case 0://init a new connection;
		break;
	case 1:// heart beat test and hall info
		HallInfo_S01 hi;
		hi.RoomNum = AllRooms.size();
		memcpy(BufSend + BufPos, &hi, sizeof(hi));
		BufPos += sizeof(hi);
		for (int i = 0; i < hi.RoomNum; i++) {
			RoomInHall_S01 ri;
			ri.PlayerNum = AllRooms[i]->PlayerNum;
			ri.RoomId = AllRooms[i]->RoomId;
			ri.P1UID = AllRooms[i]->Players[0]->UID;
			memcpy(BufSend + BufPos, &ri, sizeof(ri));
			BufPos += sizeof(ri);
		}
		break;
	case 2://return the new room info back to player;
		RoomInfo_S02 ri;
		ri.RoomId = p->CurRoom;
		memcpy(BufSend + BufPos, &ri, sizeof(ri));
		BufPos += sizeof(ri);
		break;
	case 3://return that join room successfully
		break;
	case 4://return that failed to join
		break;
	case 5://return the room inside info
	{RoomCurentInfo_S05 rci;
	Room* r = FindRoom(p->CurRoom);
	rci.RoomID = r->RoomId;
	rci.PlayerNum = r->PlayerNum;
	for (int i = 0; i < rci.PlayerNum; i++) {
		rci.UIDs[i] = r->Players[i]->UID;
	}
	memcpy(BufSend + BufPos, &rci, sizeof(rci));
	BufPos += sizeof(rci);
	break; }
	case 6: {//return the init info of game
		GameStartInfoHead_S06 gsih;
		Room* r = FindRoom(MsgNum);
		gsih.playernum = r->PlayerNum;//确定玩家数量
		memcpy(BufSend + BufPos, &gsih, sizeof(gsih));//装进数据包
		BufPos += sizeof(gsih);
		for (int i = 0; i < gsih.playernum; i++) {//装每个玩家的数据包进去
			GameStartPlayerInfo_S06 gspi;
			gspi.UID = r->Players[i]->UID;
			memcpy(BufSend + BufPos, &gspi, sizeof(gspi));//装进数据包
			BufPos += sizeof(gspi);
		}
		//传输每个房间的具体信息
		for (int i = 0; i < 9; i++) {
			GameStartDungeonInfo_S06 gsdi;
			gsdi.monnum = r->mDungeon->Monsters[i].size();
			gsdi.maptype = r->mDungeon->maptype[i];
			memcpy(BufSend + BufPos, &gsdi, sizeof(gsdi));//装进数据包
			BufPos += sizeof(gsdi);
			for (int j = 0; j < gsdi.monnum; j++) {//遍历每一个怪
				GameStartMonsterInfo_S06 gsmi;
				gsmi.type = r->mDungeon->Monsters[i][j]->type;
				gsmi.attack = r->mDungeon->Monsters[i][j]->attack;
				gsmi.curHp = r->mDungeon->Monsters[i][j]->curHp;
				gsmi.maxHp = r->mDungeon->Monsters[i][j]->maxHp;
				gsmi.MonID = r->mDungeon->Monsters[i][j]->MonID;
				gsmi.PosX = r->mDungeon->Monsters[i][j]->PosX;
				gsmi.PosY = r->mDungeon->Monsters[i][j]->PosY;
				gsmi.PosZ = r->mDungeon->Monsters[i][j]->PosZ;
				memcpy(BufSend + BufPos, &gsmi, sizeof(gsmi));//装进数据包
				BufPos += sizeof(gsmi);
			}
		}
		break; }
	case 7: {//return enter game scene;
		break;
	}
	case 8: {//game info ever frame;
		GameInfoHead_S08 gih;
		gih.playernum = FindRoom(MsgNum)->PlayerNum;
		memcpy(BufSend + BufPos, &gih, sizeof(gih));//装进数据包
		BufPos += sizeof(gih);
		for (int i = 0; i < gih.playernum; i++) {
			GamePlayerInfo_S08 gpi;
			Player* p = FindRoom(MsgNum)->Players[i];
			gpi.UID = p->UID;
			gpi.posx = p->curPosX;
			gpi.posy = p->curPosY;
			gpi.posz = p->curPosZ;
			memcpy(BufSend + BufPos, &gpi, sizeof(gpi));//装进数据包
			BufPos += sizeof(gpi);
		}
		break;
	}
	default:
		break;
	}
	ServerSend(p->PlayerSock, BufSend, BufPos);
	if (hp.CmdID != 8 && hp.CmdID != 5 && hp.CmdID != 1) {
		std::cout << "给" << hp.UID << "发送了" << hp.CmdID << "大小"<<BufPos<<std::endl;

	}
}

void JoinRoom(int RoomId, int UID) {

	Room* r = FindRoom(RoomId);
	Player* p = FindInHall(UID);
	if (r == nullptr) {
		SendMsg(p, 4, 0, nullptr);
		return;
	}
	if (r->PlayerNum < 4&&r->IsRoom) {
		r->Players.push_back(p);
		r->PlayerNum++;
		p->CurRoom = RoomId;
		SendMsg(p, 3, 0, nullptr);
		RemoveInHall(UID);
	}
	else {
		SendMsg(p, 4, 0, nullptr);
	}

}
void PlayerExitRoom(int UID) {
	Player* p = FindInAll(UID);
	Room* mRoom = FindRoom(p->CurRoom);
	mRoom->RemoveInRoom(UID);
	PlayersInHall.push_back(p);
}
Player* FindInHall(int UID) {
	for (int i = 0; i < PlayersInHall.size(); i++) {
		if (PlayersInHall[i]->UID == UID) {
			return(PlayersInHall[i]);
		}

	}
	return nullptr;
}
void RemoveInHall(int UID) {
	Player* p = FindInHall(UID);
	std::vector<Player*>::iterator iter = std::find(PlayersInHall.begin(), PlayersInHall.end(), p);
	PlayersInHall.erase(iter);
}
void RemoveInAll(int UID) {
	Player* p = FindInAll(UID);
	std::vector<Player*>::iterator iter = std::find(AllPlayers.begin(), AllPlayers.end(), p);
	AllPlayers.erase(iter);
}
Player* FindInAll(int UID) {
	for (int i = 0; i < AllPlayers.size(); i++) {
		if (AllPlayers[i]->UID == UID) {
			return(AllPlayers[i]);
		}

	}
	return nullptr;
}
Room* FindRoom(int RoomId) {
	for (int i = 0; i < AllRooms.size(); i++) {
		if (AllRooms[i]->RoomId == RoomId) {
			return AllRooms[i];
		}
	}
	return nullptr;
}
void PlayerExit(int UID) {
	Player* p = FindInAll(UID);
	if (p->CurRoom == 0) {
		RemoveInHall(UID);
	}
	else {
		FindRoom(p->CurRoom)->RemoveInRoom(UID);
	}
	RemoveInAll(UID);
	closesocket(p->PlayerSock);
	delete p;
}

int ServerRecv(SOCKET sc, char* recvMsg) {
	return (recv(sc, recvMsg, 10240, 0));
}
int ServerSend(SOCKET sc, char buf2send[], int length) {
	return(send(sc, buf2send, length, 0));
}


void ThreadForRoom(Room* mRoom) {
	int HeartBeatLastTime = Time::NowTime();
	while (true) {
		Sleep(1);
		if (mRoom->IsRoom) {
			for (int i = 0; i < mRoom->PlayerNum; i++) {
				int oriPlayerNum = mRoom->PlayerNum;
				char BufRecv[10240];
				int retVal = ServerRecv(mRoom->Players[i]->PlayerSock, BufRecv);
				if (retVal != SOCKET_ERROR) {
					ReceiveMsg(BufRecv);
					if (mRoom->PlayerNum != oriPlayerNum) {
						i--;
						break;
					}
				}
				if ((Time::NowTime() - HeartBeatLastTime) >= 1000) {
					SendMsg(mRoom->Players[i], 5, 0, nullptr);
					if (i == (mRoom->PlayerNum - 1)) {
						HeartBeatLastTime = Time::NowTime();
					}
				}
				if ((Time::NowTime() - mRoom->Players[i]->LastConnectTime) >= 15000) {
					PlayerExit(mRoom->Players[i]->UID);
					break;
				}
			}
		}
		else {//isGame
			char BufSend[10240];
			for (int i = 0; i < mRoom->PlayerNum; i++) {//receive all the cmds from clients;
				int oriPlayerNum = mRoom->PlayerNum;
				char BufRecv[10240];
				int retVal = ServerRecv(mRoom->Players[i]->PlayerSock, BufRecv);
				if (retVal != SOCKET_ERROR) {
					ReceiveMsg(BufRecv);
					if (mRoom->PlayerNum != oriPlayerNum) {
						i--;
						break;
					}
				}
				if ((Time::NowTime() - HeartBeatLastTime) >= 100) {
					SendMsg(mRoom->Players[i], 8, mRoom->RoomId, nullptr);//send curlv all player states and monster states;
					if (i == (mRoom->PlayerNum - 1)) {
						HeartBeatLastTime = Time::NowTime();
					}
				}
				if ((Time::NowTime() - mRoom->Players[i]->LastConnectTime) >= 15000) {
					PlayerExit(mRoom->Players[i]->UID);
					break;
				}
			}
		}
		if (mRoom->PlayerNum == 0) {
			RemoveRoom(mRoom->RoomId);
			break;
		}
	}

}// need to update the code to //need to write code to update players' game state;
void NewRoom(int UID) {
	Room* nr = new Room();
	Player* p = FindInHall(UID);
	if (p == nullptr) {
		return;
	}
	for (int i = 1; i < NextRoomID; i++) {
		if (FindRoom(i) == nullptr) {
			nr->RoomId = i;
			break;
		}
	}
	if (nr->RoomId == 0) {
		nr->RoomId = NextRoomID;
		NextRoomID++;
	}
	AllRooms.push_back(nr);
	nr->Players.push_back(p);
	p->CurRoom = nr->RoomId;
	RemoveInHall(UID);
	SendMsg(p, 2, 0, nullptr);
	nr->PlayerNum++;
	nr->IsRoom = true;
	SendMsg(p, 3, 0, nullptr);
	std::thread mt(ThreadForRoom, nr);
	mt.detach();
}
void RemoveRoom(int RoomId) {
	Room* r = FindRoom(RoomId);
	std::vector<Room*>::iterator iter = std::find(AllRooms.begin(), AllRooms.end(), r);
	AllRooms.erase(iter);
	delete r;
}
int main(int argc, char* argv[])
{
	Time::init();
	const int BUF_SIZE = 10240;
	WSADATA         wsd;            //WSADATA变量  
	SOCKET          sServer;        //服务器套接字  
	SOCKET          sClient;        //客户端套接字  
	std::vector<SOCKET> AllClients;		//所有客户端
	//

	int NextUID = 1;
	int HeartBeatTime = 0;
	int HeartBeatTime2 = 0;



	//
	SOCKADDR_IN     addrServ;;      //服务器地址  
	char            buf[BUF_SIZE];  //接收数据缓冲区  
	char            sendBuf[BUF_SIZE];//返回给客户端得数据  
	int             retVal;         //返回值  
									//初始化套结字动态库  
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		std::cout << "WSAStartup failed!" << std::endl;
		return 1;
	}

	//创建套接字  
	sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	u_long mode = 1;
	ioctlsocket(sServer, FIONBIO, &mode);
	if (INVALID_SOCKET == sServer)
	{
		std::cout << "socket failed!" << std::endl;
		WSACleanup();//释放套接字资源;  
		return  -1;
	}
	//服务器套接字地址   
	addrServ.sin_family = AF_INET;
	addrServ.sin_port = htons(4999);
	//addrServ.sin_addr.s_addr = inet_addr("192.168.0.13");
	addrServ.sin_addr.s_addr = INADDR_ANY;
	//绑定套接字  
	retVal = bind(sServer, (LPSOCKADDR)&addrServ, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == retVal)
	{
		std::cout << "bind failed!" << std::endl;
		closesocket(sServer);   //关闭套接字  
		WSACleanup();           //释放套接字资源;  
		return -1;
	}
	//开始监听   
	retVal = listen(sServer, 1);
	if (SOCKET_ERROR == retVal)
	{
		std::cout << "listen failed!" << std::endl;
		closesocket(sServer);   //关闭套接字  
		WSACleanup();           //释放套接字资源;  
		return -1;
	}
	//接受客户端请求  
	sockaddr_in addrClient;
	int addrClientlen = sizeof(addrClient);
	while (true)//接收玩家进入大厅循环，接收信息，群心跳
	{
		Sleep(1);
		if (Time::UpdateTime()) {
			sClient = accept(sServer, (sockaddr FAR*)&addrClient, &addrClientlen);
			if (INVALID_SOCKET != sClient)
			{
				Player* np = new Player();
				np->UID = NextUID;
				np->PlayerSock = sClient;
				NextUID++;
				PlayersInHall.push_back(np);
				np->LastConnectTime = Time::NowTime();
				np->CurRoom = 0;
				AllPlayers.push_back(np);
				std::cout << "新玩家加入" << std::endl << "总玩家数量" << AllPlayers.size() << std::endl;
				SendMsg(np, 0, 0, nullptr);
			}
			for (int i = 0; i < PlayersInHall.size(); i++) {
				int oriSize = PlayersInHall.size();
				if (Time::NowTime() - HeartBeatTime >= 1000) {
					SendMsg(PlayersInHall[i], 1, 0, nullptr);
					if (i == PlayersInHall.size() - 1) {
						HeartBeatTime = Time::NowTime();
					}
				}
				char BufRecv[10240];
				int retVal = ServerRecv(PlayersInHall[i]->PlayerSock, BufRecv);
				if (retVal != SOCKET_ERROR) {
					ReceiveMsg(BufRecv);
					if (PlayersInHall.size() != oriSize) {
						break;
					}
				}
				if (Time::NowTime() - PlayersInHall[i]->LastConnectTime >= 15000) {
					std::cout << "清除了" << PlayersInHall[i]->UID << std::endl;
					PlayerExit(PlayersInHall[i]->UID);
					break;
				}
			}
			if (Time::NowTime() - HeartBeatTime2 >= 5000) {
				HeartBeatTime2 = Time::NowTime();
			}
		}
	}
	//退出
	closesocket(sClient);   //关闭套接字  
	WSACleanup();           //释放套接字资源;  
	return 0;
}