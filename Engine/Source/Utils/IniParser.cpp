#include "pch.h"
#include "IniParser.h"

FString FIniParser::GetCurrentPath()
{
	char path[MAX_PATH] = { 0 };

	GetModuleFileNameA(NULL, path, MAX_PATH);

	return FString(path);
}
