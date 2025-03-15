#pragma once

class ICommand
{
public:
	virtual void Execute(const char*) = 0;
};