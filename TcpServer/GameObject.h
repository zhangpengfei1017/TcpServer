#pragma once
#include <string> 
using std::string;
class GameObject
{
public:
	GameObject();
	GameObject(int assetid, string name, string tag, int posX, int posY, int posZ, bool ia);
	~GameObject();
	int PosX;
	int PosY;
	int PosZ;
	int Direction = 1;
	bool isActive;
	int minX;
	int maxX;
	string name = "";
	string tag = "";
	int assetid;
};

