#include "pch.h"
#include "AbstractHack.h"

bool AbstractHack::IsActive()
{
	return active;
}

AbstractHack::AbstractHack(bool startsActive)
{
	active = startsActive;
}

void AbstractHack::SetActive(bool active)
{
	if (active)
	{
		Activate();
	}

	else
	{
		Deactivate();
	}
}

void AbstractHack::Toggle()
{
	SetActive(!active);
}

void AbstractHack::Process() 
{
	//Empty virtual function
	//Derived classes that needs constant processing should implement this
}