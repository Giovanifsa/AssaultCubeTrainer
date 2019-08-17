#include "pch.h"
#include "NoRecoil.h"
#include "HackError.h"

NoRecoil::NoRecoil() : AbstractHack(false)
{
	procHacker = new ProcessHacker(ARCH_X86);

	try 
	{
		moduleAddress = procHacker->FindModuleByName(L"ac_client.exe").modBaseAddr;
		defaultCodeInfo = procHacker->ReadRelativeMemoryLocation(moduleAddress, codeOffset, codeSize);
	}

	catch (...)
	{
		delete procHacker;
		throw;
	}
}

void NoRecoil::Activate()
{
	if (!procHacker->RemoveRelativeMemoryRegionOperation(moduleAddress, codeOffset, codeSize).hasBeenWriten)
	{
		throw HackError(L"Failed to patch recoil memory region.");
	}

	active = true;
}

void NoRecoil::Deactivate()
{
	if (!procHacker->PatchRelativeMemoryRegion(moduleAddress, codeOffset, defaultCodeInfo.dataPointer, codeSize).hasBeenWriten)
	{
		throw HackError(L"Failed to reset recoil memory region.");
	}

	active = false;
}

NoRecoil::~NoRecoil()
{
	delete procHacker;
	defaultCodeInfo.DisposeReadData();
}