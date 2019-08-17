#pragma once
#include "pch.h"
#include "HackMain.h"

//Cyclic dependency
class HackMain;

class KeyboardInput
{
private:
	HackMain* hackMain;

public:
	KeyboardInput(HackMain* hackMain);
	void ProcessInput();
};

