#include "pch.h"
#include "InfiniteAmmo.h"
#include "HackError.h"

InfiniteAmmo::InfiniteAmmo() : AbstractHack(false)
{
	procHacker = new ProcessHacker(ARCH_X86);

	moduleAddress = procHacker->FindModuleByName(L"ac_client.exe").modBaseAddr;
	defaultCodeInfo = procHacker->ReadRelativeMemoryLocation(moduleAddress, codeOffset, codeSize);

	if (!defaultCodeInfo.hasBeenRead)
	{
		throw HackError(L"Failed to read ammo memory code.");
	}
}

void InfiniteAmmo::Activate()
{
	if (!procHacker->RemoveRelativeMemoryRegionOperation(moduleAddress, codeOffset, codeSize).hasBeenWriten)
	{
		throw HackError(L"Failed to remove ammo memory code operation.");
	}

	active = true;
}

void InfiniteAmmo::Deactivate()
{
	if (!procHacker->SetRelativeMemoryLocation(moduleAddress, codeOffset, defaultCodeInfo.dataPointer, codeSize).hasBeenWriten)
	{
		throw HackError(L"Failed to reset ammo memory code operation.");
	}

	active = false;
}

InfiniteAmmo::~InfiniteAmmo()
{
	delete procHacker;
	defaultCodeInfo.DisposeReadData();
}