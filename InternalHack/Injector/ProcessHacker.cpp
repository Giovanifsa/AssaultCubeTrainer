#include "stdafx.h"
#include "ProcessHacker.h"
#include <iostream>

void ProcessHacker::LoadProcess(DWORD processID, DWORD desiredAccess, Architecture procArchitecture)
{
	if (!(hOpenProcess = OpenProcess(desiredAccess, false, processID)))
	{
		throw std::invalid_argument("Process ID not found.");
	}

	dProcessAccess = desiredAccess;
	aProcArchitecture = procArchitecture;
}

ProcessHacker::ProcessHacker(DWORD processID, DWORD desiredAccess, Architecture procArchitecture)
{
	LoadProcess(processID, desiredAccess, procArchitecture);
}

ProcessHacker::ProcessHacker(Architecture procArchitecture)
{
	//Internal hacking constructor
	aProcArchitecture = procArchitecture;
}

ProcessHacker::ProcessHacker(const WCHAR* fileName, DWORD desiredAccess, Architecture procArchitecture)
{
	HANDLE th32 = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (th32 != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32W procEntry32 = { 0 };
		procEntry32.dwSize = sizeof(PROCESSENTRY32W);

		try
		{
			if (Process32FirstW(th32, &procEntry32))
			{
				do
				{
					if (wcscmp(procEntry32.szExeFile, fileName) == 0)
					{
						LoadProcess(procEntry32.th32ProcessID, desiredAccess, procArchitecture);
						return;
					}
				} while (Process32NextW(th32, &procEntry32));
			}
		}

		catch (...)
		{
			CloseHandle(th32);
			throw;
		}

		CloseHandle(th32);
	}

	else
	{
		throw std::runtime_error("Failed to initialize CreateToolhelp32Snapshot.");
	}

	throw std::invalid_argument("Process not found.");
}

std::vector<MODULEENTRY32W> ProcessHacker::EnumerateModules()
{
	HANDLE th32 = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32,
		(hOpenProcess ? GetProcessId(hOpenProcess) : GetCurrentProcessId()));

	if (th32 != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32W modEntry32 = { 0 };
		modEntry32.dwSize = sizeof(MODULEENTRY32W);

		std::vector<MODULEENTRY32> moduleList;

		if (Module32FirstW(th32, &modEntry32))
		{
			do
			{
				moduleList.push_back(modEntry32);
			} while (Module32NextW(th32, &modEntry32));
		}

		CloseHandle(th32);
		return moduleList;
	}

	else
	{
		throw std::runtime_error("Failed to initialize CreateToolhelp32Snapshot.");
	}
}

WritenInfo ProcessHacker::SetMemoryLocation(LPVOID address, LPVOID data, SIZE_T dataSize)
{
	WritenInfo info = { 0 };
	info.writenAddress = address;
	info.dataWriten = data;
	info.requestedBytes = dataSize;

	if (hOpenProcess)
	{
		info.hasBeenWriten = WriteProcessMemory(hOpenProcess, address, data, dataSize, &info.totalBytesWriten);
	}

	else if (memcpy(address, data, dataSize))
	{
		info.hasBeenWriten = true;
		info.totalBytesWriten = dataSize;
	}

	return info;
}

ReadInfo ProcessHacker::ReadMemoryLocation(LPVOID address, SIZE_T readLength)
{
	ReadInfo info = { 0 };
	char* buffer = new char[readLength];
	info.requestedBytes = readLength;
	info.readAddress = address;

	if (hOpenProcess)
	{
		if (info.hasBeenRead = ReadProcessMemory(hOpenProcess, address, buffer, readLength, &info.totalBytesRead))
		{
			info.dataPointer = (LPVOID)buffer;
		}

		else
		{
			delete[] buffer;
		}
	}

	else if (memcpy((LPVOID)buffer, address, readLength))
	{
		info.hasBeenRead = true;
		info.totalBytesRead = readLength;
		info.dataPointer = buffer;
	}

	return info;
}

WritenInfo ProcessHacker::SetRelativeMemoryLocation(LPVOID address, int offset, LPVOID data, SIZE_T dataSize)
{
	return SetMemoryLocation((LPVOID)(((char*)address) + offset), data, dataSize);
}

ReadInfo ProcessHacker::ReadRelativeMemoryLocation(LPVOID address, int offset, SIZE_T readLength)
{
	return ReadMemoryLocation((LPVOID)(((char*)address) + offset), readLength);
}

DWORD ProcessHacker::ChangeProcessAccess(DWORD desiredAccess)
{
	if (hOpenProcess)
	{
		HANDLE newProcess = OpenProcess(desiredAccess, false, GetProcessId(hOpenProcess));

		if (newProcess)
		{
			hOpenProcess = newProcess;
			DWORD oldAccess = dProcessAccess;
			dProcessAccess = desiredAccess;

			return oldAccess;
		}

		throw std::runtime_error("Failed to change process access.");
	}

	throw std::logic_error("Cannot change internal process access.");
}

void ProcessHacker::ProcLoadLibraryA(const char* libraryPath)
{
	if (hOpenProcess)
	{
		LPVOID address = AllocateProtectedMemory(strlen(libraryPath) + 1, PAGE_READWRITE);

		if (address)
		{
			if (SetMemoryLocation(address, (LPVOID)libraryPath, strlen(libraryPath) + 1).hasBeenWriten)
			{
				FARPROC procAddress = GetProcAddress(GetModuleHandle(L"KERNEL32.DLL"), "LoadLibraryA");

				if (procAddress)
				{
					HANDLE hThread = CreateRemoteThread(hOpenProcess, NULL, NULL,
						(LPTHREAD_START_ROUTINE)procAddress, address, NULL, NULL);

					if (hThread)
					{
						//Waits for library load
						WaitForSingleObject(hThread, INFINITE);

						//Release path memory
						FreeProtectedMemory(address);
					}

					else
					{
						FreeProtectedMemory(address);
						throw std::runtime_error("Failed to create remote thread.");
					}
				}

				else
				{
					FreeProtectedMemory(address);
					throw std::runtime_error("Failed to find LoadLibraryW address.");
				}
			}

			else
			{
				FreeProtectedMemory(address);
				throw std::runtime_error("Failed to write library path.");
			}
		}

		else
		{
			throw std::runtime_error("Memory allocation failed.");
		}
	}

	else
	{
		LoadLibraryA(libraryPath);
	}
}

MODULEENTRY32W ProcessHacker::FindModuleByName(const WCHAR* moduleName)
{
	std::vector<MODULEENTRY32W> moduleList = EnumerateModules();

	for (MODULEENTRY32W i : moduleList)
	{
		if (wcscmp(i.szModule, moduleName) == 0)
		{
			return i;
		}
	}

	throw std::logic_error("Module not found.");
}

ReadInfo ProcessHacker::ReadMultiDimensionPointerMemory(LPVOID address, int* offsets, SIZE_T offsetsLength, SIZE_T readLength)
{
	return ReadMemoryLocation(MultiDimensionPointerResolver(address, offsets, offsetsLength), readLength);
}

WritenInfo ProcessHacker::SetMultiDimensionPointerMemory(LPVOID address, int* offsets, SIZE_T offsetsLength, LPVOID data, SIZE_T writeLength)
{
	return SetMemoryLocation(MultiDimensionPointerResolver(address, offsets, offsetsLength), data, writeLength);
}

LPVOID ProcessHacker::MultiDimensionPointerResolver(LPVOID initialAddress, int* offsets, SIZE_T offsetsLength)
{
	LPVOID currentAddress = initialAddress;
	SIZE_T pointerSize = (aProcArchitecture == ARCH_X86 ? sizeof(unsigned int) : sizeof(unsigned long long int));

	for (SIZE_T offsetIterator = 0; offsetIterator < offsetsLength - 1; offsetIterator++)
	{
		ReadInfo info = ReadRelativeMemoryLocation(currentAddress, *(offsets + offsetIterator), pointerSize);

		if (info.hasBeenRead && info.totalBytesRead == pointerSize)
		{
			if (pointerSize == sizeof(unsigned int))
			{
				//Get address being pointed by the pointer that has been read
				currentAddress = (LPVOID)(*(unsigned int*)info.dataPointer);
			}

			else
			{
				//For 64bits
				currentAddress = (LPVOID)(*(unsigned long long int*)info.dataPointer);
			}
		}

		else
		{
			throw std::runtime_error("Failed to resolve multi-dimension pointers.");
		}
	}

	return (LPVOID)(((char*)currentAddress) + *(offsets + (offsetsLength - 1)));
}

LPVOID ProcessHacker::AllocateProtectedMemory(SIZE_T size, DWORD pageProtection)
{
	if (hOpenProcess)
	{
		return VirtualAllocEx(hOpenProcess, NULL, size, MEM_RESERVE | MEM_COMMIT, pageProtection);
	}

	return VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT, pageProtection);
}

LPVOID ProcessHacker::AllocateReadWriteMemory(SIZE_T size)
{
	return AllocateProtectedMemory(size, PAGE_READWRITE);
}

LPVOID ProcessHacker::AllocateExecuteReadWriteMemory(SIZE_T size)
{
	return AllocateProtectedMemory(size, PAGE_EXECUTE_READWRITE);
}

void ProcessHacker::FreeProtectedMemory(LPVOID address)
{
	if (hOpenProcess)
	{
		VirtualFreeEx(hOpenProcess, address, NULL, MEM_RELEASE);
	}

	else
	{
		VirtualFree(address, NULL, MEM_RELEASE);
	}
}

WritenInfo ProcessHacker::PatchMemoryRegion(LPVOID address, LPVOID data, SIZE_T dataSize)
{
	DWORD oldProtection = SetMemoryProtection(address, dataSize, PAGE_EXECUTE_READWRITE);
	WritenInfo info = SetMemoryLocation(address, data, dataSize);
	SetMemoryProtection(address, dataSize, oldProtection);

	if (info.hasBeenWriten)
	{
		return info;
	}

	throw std::runtime_error("Failed to patch memory region.");
}

WritenInfo ProcessHacker::PatchRelativeMemoryRegion(LPVOID address, int offset, LPVOID data, SIZE_T dataSize)
{
	return PatchMemoryRegion((LPVOID)(((char*)address) + offset), data, dataSize);
}

WritenInfo ProcessHacker::PatchMultiDimensionPointerRegion(LPVOID address, int* offsets, SIZE_T offsetsLength, LPVOID data, SIZE_T dataSize)
{
	return PatchMemoryRegion(MultiDimensionPointerResolver(address, offsets, offsetsLength), data, dataSize);
}

DWORD ProcessHacker::SetMemoryProtection(LPVOID address, SIZE_T changeSize, DWORD pageProtection)
{
	DWORD oldProtection;

	if (hOpenProcess)
	{
		if (VirtualProtectEx(hOpenProcess, address, changeSize, pageProtection, &oldProtection))
		{
			return oldProtection;
		}
	}

	else
	{
		if (VirtualProtect(address, changeSize, pageProtection, &oldProtection))
		{
			return oldProtection;
		}
	}

	throw std::runtime_error("Failed to change memory region protection.");
}

WritenInfo ProcessHacker::RemoveMemoryRegionOperation(LPVOID address, SIZE_T regionSize)
{
	char* data = new char[regionSize];

	for (SIZE_T i = 0; i < regionSize; i++)
	{
		(*(data + i)) = (char)0x90; //NOP
	}

	WritenInfo info = PatchMemoryRegion(address, (LPVOID)data, regionSize);
	info.dataWriten = nullptr;

	return info;
}

WritenInfo ProcessHacker::RemoveRelativeMemoryRegionOperation(LPVOID address, int offset, SIZE_T regionSize)
{
	return RemoveMemoryRegionOperation((LPVOID)(((char*)address) + offset), regionSize);
}

bool ProcessHacker::IsProcessAlive()
{
	DWORD exitCode;
	return (GetExitCodeProcess(hOpenProcess, &exitCode) && exitCode == STILL_ACTIVE);
}

ProcessHacker::~ProcessHacker()
{
	CloseHandle(hOpenProcess);
}