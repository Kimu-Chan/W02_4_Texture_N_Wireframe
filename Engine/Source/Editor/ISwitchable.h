#pragma once
class ISwitchable
{
public:
	virtual void Toggle() = 0;

	virtual ~ISwitchable() {}
};

