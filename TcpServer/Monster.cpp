#include "Monster.h"



Monster::Monster()
{
}


Monster::~Monster()
{
}

void Monster::takeattack(int damage)
{
	curHp = (curHp - damage) < 0 ? 0 : (curHp - damage);
}
