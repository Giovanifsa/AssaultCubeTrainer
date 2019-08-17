#pragma once
class AbstractHack
{
protected:
	bool active = false;

public:
	AbstractHack(bool startsActive);

	virtual void Activate() = 0;
	virtual void Deactivate() = 0;
	virtual void SetActive(bool active);
	virtual void Toggle();
	virtual void Process();
	bool IsActive();
};

