#include "Dungeon.h"



Dungeon::Dungeon()
{
}


Dungeon::~Dungeon()
{
}

void Dungeon::NewDungeon(int num) {
	//1随机最终boss所在位置
	//2随机起点
	//3随机全图路线
	//4随机每个房间的怪物
	int bossroom = 6 + (rand() % 3);
	for (int i = 0; i < 9; i++) {
		//random monster in room
		//int monnum = num + (rand() % 3);
		int monnum = 5;
		for (int j = 0; j < monnum; j++) {
			Monster* mon = new Monster();
			mon->PosX = 180 + (rand() % 360);
			mon->PosZ = 10 + (rand() % 120);
			mon->PosY = 0;
			mon->MonID = j;
			mon->type = 10;
			mon->maxHp = 500+num*200;
			mon->curHp = mon->maxHp;
			mon->attack = 20;
			Monsters[i].push_back(mon);
		}
	}
	//random boos in a certain room
	Monster* boss = new Monster();
	boss->MonID = Monsters[bossroom].size();
	boss->attack = 60;
	boss->maxHp = 3000+num*1200;
	boss->curHp = boss->maxHp;
	boss->PosX = 540;
	boss->PosY = 0;
	boss->PosZ = 70;
	boss->type = 10;
	Monsters[bossroom].push_back(boss);
}
