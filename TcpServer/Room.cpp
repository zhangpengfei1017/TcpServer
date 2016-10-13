#include "Room.h"



Room::Room()
{
}


Room::~Room()
{
}


void Room::RemoveInRoom(int UID) {
	Player* p = FindInRoom(UID);
	p->CurRoom = 0;
	std::vector<Player*>::iterator iter = std::find(Players.begin(), Players.end(), p);
	Players.erase(iter);
	PlayerNum--;
	
}
Player* Room::FindInRoom(int UID) {
	for (int i = 0; i < Players.size(); i++) {
		if (Players[i]->UID == UID) {
			return Players[i];
		}
	}
	return nullptr;
}