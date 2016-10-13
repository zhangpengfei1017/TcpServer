#pragma once
#include "Monster.h"
#include <vector>
using std::vector;
class Dungeon
{//包含每一次游戏所需要的地图、怪物等信息
public:
	Dungeon();
	~Dungeon();
	vector<Monster*> Monsters[9];
	int maptype[9] = { 5,9,6,10,13,11,7,12,8 };
	int curDungeon = 0;
	void NewDungeon(int num);

};

