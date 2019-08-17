#pragma once
#include "pch.h"
#include "AbstractHack.h"
#include "ProcessHacker.h"

class InfiniteAmmo : public AbstractHack
{
private:
	LPVOID moduleAddress;
	const int codeOffset = 0x637E9;
	const SIZE_T codeSize = 2;

	ReadInfo defaultCodeInfo;
	ProcessHacker* procHacker;
public:
	void Activate() override;
	void Deactivate() override;

	InfiniteAmmo();
	~InfiniteAmmo();
};

