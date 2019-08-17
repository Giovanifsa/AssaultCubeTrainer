#pragma once
#include "pch.h"
#include "AbstractHack.h"
#include "ProcessHacker.h"

class NoRecoil : public AbstractHack
{
private:
	LPVOID moduleAddress;
	const int codeOffset = 0x63786;
	const SIZE_T codeSize = 10;

	ReadInfo defaultCodeInfo;
	ProcessHacker* procHacker;

public:
	NoRecoil();
	~NoRecoil();

	void Activate() override;
	void Deactivate() override;
};