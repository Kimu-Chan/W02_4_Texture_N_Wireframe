#include "pch.h"
#include "EngineConfig.h"
#include "IniParser/iniparser.hpp"

FEngineConfig::FEngineConfig()
{
	Path = "engine.ini";
	ft.Load(*Path);
}

FEngineConfig::~FEngineConfig()
{
	ft.Unload();
}

void FEngineConfig::LoadEngineConfig()
{
	for (auto& SectionMapping : SectionMappings)
	{
		ft.GetSection(*SectionMapping.Key);
		for (auto& ConfigMapping : ConfigMappings)
		{
			switch (ConfigMapping.ValueType)
			{
				FString Value = ft.GetValue(*ConfigMapping.Key).AsString();
				EngineConfig[SectionMapping.Section][ConfigMapping.Config] = Value;
			}
		}
	}
}

void FEngineConfig::SaveAllConfig()
{
	for (auto& SectionMapping : SectionMappings)
	{
		ft.GetSection(*SectionMapping.Key);
		for (auto& ConfigMapping : ConfigMappings)
		{
			ft.SetValue(*ConfigMapping.Key, EngineConfig[SectionMapping.Section][ConfigMapping.Config]);
		}
	}

	ft.Save(*Path);
}

EEngineConfigSectionType FEngineConfig::FindSection(const EEngineConfigValueType& InValueType) const
{
	for (const auto& configMapping : ConfigMappings)
	{
		if (configMapping.Config == InValueType)
		{
			return configMapping.Section;
		}
	}
	return EEngineConfigSectionType::ECS_None;
}

FString FEngineConfig::GetPath()
{
	char Path[MAX_PATH];
	DWORD Result = GetModuleFileNameA(NULL, Path, MAX_PATH);

	FString PathStr(Path);

	PathStr += "\\engine.ini";
	return PathStr;
}
