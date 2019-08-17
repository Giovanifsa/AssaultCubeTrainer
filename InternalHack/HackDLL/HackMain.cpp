#include "pch.h"
#include "HackMain.h"

DWORD HackMain::ProcessingThread(LPVOID paramData)
{
	HackMain* hackMain = (HackMain*)paramData;

	while (!hackMain->flagUnloading)
	{
		hackMain->ProcessHack();
	}

	HMODULE hModule = hackMain->hackModule;
	delete hackMain;

	FreeLibraryAndExitThread(hModule, 0);
}

HackMain::HackMain(HMODULE hackModule)
{
	this->hackModule = hackModule;

	infiniteAmmoHack = new InfiniteAmmo();
	infiniteHealthHack = new InfiniteHealth();
	noRecoilHack = new NoRecoil();

	keyboardInput = new KeyboardInput(this);

	CreateThread(NULL, NULL, HackMain::ProcessingThread, (LPVOID)this, NULL, NULL);
}

void HackMain::ProcessHack()
{
	infiniteAmmoHack->Process();
	infiniteHealthHack->Process();
	noRecoilHack->Process();

	keyboardInput->ProcessInput();
}

void HackMain::UnloadHack()
{
	flagUnloading = true;
}

HackMain::~HackMain()
{
	delete keyboardInput;
	delete infiniteAmmoHack;
	delete infiniteHealthHack;
	delete noRecoilHack;
}
