#include "pch.h"
#include "InfiniteHealth.h"
#include <fstream>

InfiniteHealth::InfiniteHealth() : AbstractHack(false)
{
	procHacker = new ProcessHacker(ARCH_X86);
	moduleAddress = procHacker->FindModuleByName(L"ac_client.exe").modBaseAddr;
}

void InfiniteHealth::Activate()
{
	active = true;
}

void InfiniteHealth::Deactivate()
{
	active = false;
}

void InfiniteHealth::Process()
{
	if (active)
	{
		static const int health = 100;
		WritenInfo info = procHacker->SetMultiDimensionPointerMemory(moduleAddress, healthOffsets, offsetCount, (LPVOID)&health, sizeof(int));

		if (!info.hasBeenWriten)
		{
			throw HackError(L"Failed to write health to memory.");
		}
	}
}

InfiniteHealth::~InfiniteHealth()
{
	delete procHacker;
}
