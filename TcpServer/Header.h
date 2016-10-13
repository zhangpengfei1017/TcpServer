#pragma once
#include "winsock2.h"  
#pragma comment(lib, "ws2_32.lib")  
#include <iostream>  
#include <vector>
#include <thread>
#include "GameObject.h"
#include "Room.h"
#include "Player.h"
#include <fstream>
#include "Time.h"
void ReceiveMsg(char buf[]);
void SendMsg(Player* p, int cmdId, int MsgNum, char MsgChar[]);
void JoinRoom(int RoomId, int UID);
void PlayerExitRoom(int UID);
Player* FindInHall(int UID);
void RemoveInHall(int UID);
void RemoveInAll(int UID);
Player* FindInAll(int UID);
Room* FindRoom(int RoomId);
void PlayerExit(int UID);
int ServerRecv(SOCKET sc, char* recvMsg);
int ServerSend(SOCKET sc, char buf2send[],int length);
void ThreadForRoom(Room* mRoom);
void NewRoom(int UID);
void RemoveRoom(int RoomId);