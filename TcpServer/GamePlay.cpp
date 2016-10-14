#include "GamePlay.h"



GamePlay::GamePlay()
{
}


GamePlay::~GamePlay()
{
}

int GamePlay::Attack(int type, int x, int y, int z, int dirt, int skill,Room* r)
{
	int id = -1;
	if (type == 0)//player to monster
	{
		
		for (int i = 0; i < r->mDungeon->Monsters[r->mDungeon->curDungeon].size(); i++) {
			int disY = r->mDungeon->Monsters[r->mDungeon->curDungeon][i]->PosY - y;
			if (disY > -5 && disY < 5) {
				int disZ = r->mDungeon->Monsters[r->mDungeon->curDungeon][i]->PosZ - z;
				if (disZ > -5 && disZ < 5) {
					int disX = dirt*(r->mDungeon->Monsters[r->mDungeon->curDungeon][i]->PosX-x) ;
					if (disX<30&&disX>0) {
						r->mDungeon->Monsters[r->mDungeon->curDungeon][i]->takeattack(100);
						id = r->mDungeon->Monsters[r->mDungeon->curDungeon][i]->MonID;
					}
				}
			}
		}
	}
	return(id);
}
