#include "pch.h"
#include "KeyboardInput.h"
#include <Windows.h>

KeyboardInput::KeyboardInput(HackMain* hackMain)
{
	this->hackMain = hackMain;
}

void KeyboardInput::ProcessInput()
{
	if (GetAsyncKeyState(VK_NUMPAD4) & 1)
	{
		hackMain->infiniteAmmoHack->Toggle();
	}

	if (GetAsyncKeyState(VK_NUMPAD5) & 1)
	{
		hackMain->infiniteHealthHack->Toggle();
	}

	if (GetAsyncKeyState(VK_NUMPAD6) & 1)
	{
		hackMain->noRecoilHack->Toggle();
	}

	if (GetAsyncKeyState(VK_NUMPAD9) & 1)
	{
		hackMain->UnloadHack();
	}
}
