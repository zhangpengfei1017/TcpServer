#pragma once
#include "Player.h" 
#include <vector>
#include "Dungeon.h"
class Room
{
public:
	Room();
	~Room();
	int RoomId=0;
	int PlayerNum=0;
	
	std::vector<Player*> Players;
	int ReadyNum = 0;
	bool IsRoom;
	Dungeon* mDungeon=new Dungeon();
	Player* FindInRoom(int UID);
	void RemoveInRoom(int UID);
};

