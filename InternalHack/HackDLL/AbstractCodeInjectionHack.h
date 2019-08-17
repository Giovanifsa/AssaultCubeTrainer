#pragma once
#include "pch.h"
#include "AbstractHack.h"
#include "Proces"

class AbstractCodeInjectionHack : public AbstractHack
{
protected:
	LPVOID moduleAddress;
	int codeOffset;
	SIZE_T codeSize;
	ReadInfo defaultCodeInfo;

	AbstractCodeInjectionHack(bool startsActive);
public:
	~AbstractCodeInjectionHack();
};

