#pragma once
#include "pch.h"
#include <Windows.h>
#include "ProcessHacker.h"
#include "AbstractHack.h"
#include "InfiniteAmmo.h"
#include "InfiniteHealth.h"
#include "NoRecoil.h"
#include "KeyboardInput.h"

//Cyclic dependency
class KeyboardInput;

class HackMain
{
private:
	volatile bool flagUnloading = false;
	
	KeyboardInput* keyboardInput;

	static DWORD WINAPI ProcessingThread(LPVOID paramData);

public:
	AbstractHack* infiniteAmmoHack;
	AbstractHack* infiniteHealthHack;
	AbstractHack* noRecoilHack;
	HMODULE hackModule;

	void ProcessHack();
	void UnloadHack();

	HackMain(HMODULE hackModule);
	~HackMain();
};

