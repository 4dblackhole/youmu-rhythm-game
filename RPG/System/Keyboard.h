#pragma once
#include "Singleton/SingletonBase.h"

#define MAX_INPUT_KEY 256
#define MAX_INPUT_MOUSE 8

class Keyboard : public SingletonBase<Keyboard>
{
public:
	void Update();

	bool Down(DWORD key) const { return keyMap[key] == KEY_INPUT_STATUS_DOWN; }
	bool Up(DWORD key) const { return keyMap[key] == KEY_INPUT_STATUS_UP; }
	bool Hold(DWORD key) const { return keyMap[key] == KEY_INPUT_STATUS_PRESS; }

	Keyboard();
	~Keyboard();

private:

	byte keyState[MAX_INPUT_KEY];
	byte keyOldState[MAX_INPUT_KEY];
	byte keyMap[MAX_INPUT_KEY];

	enum
	{
		KEY_INPUT_STATUS_NONE = 0,
		KEY_INPUT_STATUS_DOWN,
		KEY_INPUT_STATUS_UP,
		KEY_INPUT_STATUS_PRESS,
	};
};

#define KEYBOARD (Keyboard::GetInstance())