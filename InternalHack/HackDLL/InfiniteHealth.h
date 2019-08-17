#pragma once
#include "pch.h"
#include "AbstractHack.h"
#include "ProcessHacker.h"
#include "HackError.h"

class InfiniteHealth : public AbstractHack
{
private:
	LPVOID moduleAddress;
	int healthOffsets[2] = { 0x10F4F4, 0xF8 };
	const SIZE_T offsetCount = 2;
	ProcessHacker* procHacker;

public:
	void Activate() override;
	void Deactivate() override;
	void Process() override;

	InfiniteHealth();
	~InfiniteHealth();
};

