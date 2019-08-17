#pragma once
#include "stdafx.h"
#include <Windows.h>
#include <vector>
#include <TlHelp32.h>
#include <stdexcept>
#include <cstdarg>

struct WritenInfo
{
	LPVOID dataWriten;
	SIZE_T totalBytesWriten;
	BOOL hasBeenWriten;

	SIZE_T requestedBytes;
	LPVOID writenAddress;
};

struct ReadInfo
{
	SIZE_T totalBytesRead;
	LPVOID dataPointer;
	BOOL hasBeenRead;

	SIZE_T requestedBytes;
	LPVOID readAddress;

	void DisposeReadData()
	{
		delete[] dataPointer;
	}
};

enum Architecture
{
	ARCH_X86 = 0,
	ARCH_AMD64 = 1,
};

class ProcessHacker
{
private:
	HANDLE hOpenProcess = NULL;
	DWORD dProcessAccess = NULL;
	Architecture aProcArchitecture;

	void LoadProcess(DWORD processID, DWORD desiredAccess, Architecture procArchitecture);
public:
	ProcessHacker(DWORD processID, DWORD desiredAccess, Architecture procArchitecture);
	ProcessHacker(const WCHAR* fileName, DWORD desiredAccess, Architecture procArchitecture);
	ProcessHacker(Architecture procArchitecture);

	std::vector<MODULEENTRY32W> EnumerateModules();

	WritenInfo SetMemoryLocation(LPVOID address, LPVOID data, SIZE_T dataSize);
	WritenInfo SetMultiDimensionPointerMemory(LPVOID address, int* offsets, SIZE_T offsetsLength, LPVOID data, SIZE_T writeLength);
	WritenInfo SetRelativeMemoryLocation(LPVOID address, int offset, LPVOID data, SIZE_T dataSize);

	LPVOID AllocateProtectedMemory(SIZE_T size, DWORD pageProtection);
	LPVOID AllocateReadWriteMemory(SIZE_T size);
	LPVOID AllocateExecuteReadWriteMemory(SIZE_T size);
	void FreeProtectedMemory(LPVOID address);

	WritenInfo PatchMemoryRegion(LPVOID address, LPVOID data, SIZE_T dataSize);
	WritenInfo PatchRelativeMemoryRegion(LPVOID address, int offset, LPVOID data, SIZE_T dataSize);
	WritenInfo PatchMultiDimensionPointerRegion(LPVOID address, int* offsets, SIZE_T offsetsLength, LPVOID data, SIZE_T dataSize);

	WritenInfo RemoveMemoryRegionOperation(LPVOID address, SIZE_T regionSize);
	WritenInfo RemoveRelativeMemoryRegionOperation(LPVOID address, int offset, SIZE_T regionSize);

	DWORD SetMemoryProtection(LPVOID address, SIZE_T changeSize, DWORD pageProtection);

	ReadInfo ReadMemoryLocation(LPVOID address, SIZE_T readLength);
	ReadInfo ReadMultiDimensionPointerMemory(LPVOID address, int* offsets, SIZE_T offsetsLength, SIZE_T readLength);
	ReadInfo ReadRelativeMemoryLocation(LPVOID address, int offset, SIZE_T readLength);

	MODULEENTRY32W FindModuleByName(const WCHAR* moduleName);

	DWORD ChangeProcessAccess(DWORD desiredAccess);
	LPVOID MultiDimensionPointerResolver(LPVOID initialAddress, int* offsets, SIZE_T offsetsLength);

	void ProcLoadLibraryA(const char* libraryPath);

	bool IsProcessAlive();

	virtual ~ProcessHacker();
};