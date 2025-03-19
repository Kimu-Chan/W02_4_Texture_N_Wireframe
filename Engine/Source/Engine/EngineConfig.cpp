#include "pch.h"
#include "EngineConfig.h"
#include "IniParser/iniparser.hpp"

FEngineConfig::FEngineConfig()
{
	Path = TEXT("engine.ini");
	std::string PathStr(Path.c_char());
	ft.Load(PathStr);
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

		std::string SectionName(SectionMapping.Key.c_char());
		INI::Section* Section = ft.GetSection(SectionName);

		TArray<ConfigMapping> ConfigList = GetConfigList(SectionMapping.Section);
		for (auto& Config : ConfigList)
		{
			FString Value = Section->GetValue(std::string(Config.Key.c_char())).AsString();

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
		INI::Section* Section = ft.GetSection(std::string(SectionMapping.Key.c_char()));
		for (auto& ConfigMapping : ConfigMappings)
		{
			FString Value = EngineConfig[SectionMapping.Section][ConfigMapping.Config];
			if (Value.IsEmpty() || Value == "")
				continue;

			Section->SetValue(std::string(ConfigMapping.Key.c_char()), EngineConfig[SectionMapping.Section][ConfigMapping.Config]);
		}
	}

	ft.Save(std::string(Path.c_char()));
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
