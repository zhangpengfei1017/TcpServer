#pragma once
#include "Room.h"

class GamePlay
{
public:
	GamePlay();
	~GamePlay();

	void static Attack(int type, int x, int y, int z, int dirt,int skill,Room* r);
};

