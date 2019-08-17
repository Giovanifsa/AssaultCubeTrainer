#pragma once
#include "pch.h"
#include <Windows.h>

class HackError
{
private:
	const WCHAR* message;

public:
	HackError(const WCHAR* message);
	const WCHAR* GetMessage();
};

