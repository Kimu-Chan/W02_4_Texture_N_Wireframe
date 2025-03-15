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
		if (SectionMapping.Section == EEngineConfigSectionType::ECS_None)
			continue;

		INI::Section* Section = ft.GetSection(*SectionMapping.Key);

		TArray<ConfigMapping> ConfigList = GetConfigList(SectionMapping.Section);
		for (auto& Config : ConfigList)
		{
			FString Value = Section->GetValue(*Config.Key).AsString();

			if (Value.IsEmpty() || Value == "")
				continue;
			EngineConfig[SectionMapping.Section][Config.Config] = Value;
		}
	}
}

void FEngineConfig::SaveAllConfig()
{
	for (auto& SectionMapping : SectionMappings)
	{
		INI::Section* Section = ft.GetSection(*SectionMapping.Key);
		for (auto& ConfigMapping : ConfigMappings)
		{
			FString Value = EngineConfig[SectionMapping.Section][ConfigMapping.Config];
			if (Value.IsEmpty() || Value == "")
				continue;

			Section->SetValue(*ConfigMapping.Key, EngineConfig[SectionMapping.Section][ConfigMapping.Config]);
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

TArray<ConfigMapping> FEngineConfig::GetConfigList(const EEngineConfigSectionType& InSectionType) const
{
	TArray<ConfigMapping> ConfigList;
	for (const auto& configMapping : ConfigMappings)
	{
		if (configMapping.Section == InSectionType)
		{
			ConfigList.Add(configMapping);
		}
	}

	return ConfigList;
}

FString FEngineConfig::GetPath()
{
	char Path[MAX_PATH];
	DWORD Result = GetModuleFileNameA(NULL, Path, MAX_PATH);

	FString PathStr(Path);

	PathStr += "\\engine.ini";
	return PathStr;
}
