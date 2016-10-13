#pragma once
#include <WinSock2.h>
class Player
{
public:
	Player();
	~Player();

	//
	int UID;
	SOCKET PlayerSock;
	int CurRoom;
	int LastConnectTime=0;

	int curPosX=160;
	int curPosY=0;
	int curPosZ=20;
	int MaxHp;
	int MaxMp;
};

