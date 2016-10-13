#pragma once
class Time
{
public:
	Time();
	~Time();
	static int LastFrameTime;
	static int FramesCount;
	static float deltaTime;
	static bool UpdateTime();
	static int NowTime();
	static void init();
};

