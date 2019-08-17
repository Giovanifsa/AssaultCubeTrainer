#include "pch.h"
#include "HackError.h"

HackError::HackError(const WCHAR* message)
{
	this->message = message;
}

const WCHAR* HackError::GetMessage()
{
	return message;
}