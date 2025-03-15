#pragma once
#define DEFAULT_INI_FILE_PATH "\\engine.ini"

#define SECTION_MAPPING(Section, Key) { EEngineConfigSectionType::Section, Key }
#define CONFIG_MAPPING(Config, Key, ValueType, SectionType) { EEngineConfigValueType::Config, Key, EConfigValueType::ValueType, EEngineConfigSectionType::SectionType }
#include "Core/Container/String.h"
#include "Core/Container/Map.h"
#include "ThirdParty/IniParser/iniparser.hpp"

enum class EEngineConfigSectionType
{
	ECS_None,

	ECS_Screen,
	ECS_Camera,
	ECS_Max,
};


enum class EEngineConfigValueType
{
	EEC_None,
	// 화면
	EEC_ScreenWidth,
	EEC_ScreenHeight,

	// 에디터
	EEC_EditorCameraPosX,
	EEC_EditorCameraPosY,
	EEC_EditorCameraPosZ,

	EEC_EditorCameraRotX,
	EEC_EditorCameraRotY,
	EEC_EditorCameraRotZ,

	EEC_EditorCameraSpeed,

	EEC_Max,
};

enum class EConfigValueType
{
	Int,
	Float,
	String,
};

struct SectionMapping
{
	EEngineConfigSectionType Section;
	const FString Key;
};

struct ConfigMapping
{
	EEngineConfigValueType Config;
	const FString Key;
	// 값의 타입
	EConfigValueType ValueType;
	EEngineConfigSectionType Section;
};

static const SectionMapping SectionMappings[] =
{
	SECTION_MAPPING(ECS_None, "NONE"),
	SECTION_MAPPING(ECS_Screen, "Screen"),
	SECTION_MAPPING(ECS_Camera, "Camera")
	// !TODO : EngineConfigSection 추가시 추가
};

static const ConfigMapping ConfigMappings[] = {
	CONFIG_MAPPING(EEC_None, "None", Int, ECS_None),
	CONFIG_MAPPING(EEC_ScreenWidth, "ScreenWidth", Int, ECS_Screen),
	CONFIG_MAPPING(EEC_ScreenHeight, "ScreenHeight", Int, ECS_Screen),

	CONFIG_MAPPING(EEC_EditorCameraPosX, "EditorCameraPosX", Float, ECS_Camera),
	CONFIG_MAPPING(EEC_EditorCameraPosY, "EditorCameraPosY", Float, ECS_Camera),
	CONFIG_MAPPING(EEC_EditorCameraPosZ, "EditorCameraPosZ", Float, ECS_Camera),

	CONFIG_MAPPING(EEC_EditorCameraRotX, "EditorCameraRotX", Float, ECS_Camera),
	CONFIG_MAPPING(EEC_EditorCameraRotY, "EditorCameraRotY", Float, ECS_Camera),
	CONFIG_MAPPING(EEC_EditorCameraRotZ, "EditorCameraRotZ", Float, ECS_Camera),

	CONFIG_MAPPING(EEC_EditorCameraSpeed, "EditorCameraSpeed", Float, ECS_Camera)
	// !TODO : EngineConfig 추가시 추가
};

class UEngine;

class FEngineConfig
{
	friend class UEngine;

private:
	FEngineConfig();
	~FEngineConfig();

public:
	void LoadEngineConfig();
	template<typename T>
	void SaveEngineConfig(EEngineConfigValueType InConfig, T InValue)
	{
		auto Section = FindSection(InConfig);
		if (Section == EEngineConfigSectionType::ECS_None)
		{
			return;
		}
		EngineConfig[Section][InConfig] = FString::SanitizeFloat(InValue);
		
		auto Section_ft = ft.GetSection(*SectionMappings[static_cast<int>(Section)].Key);

		if (Section_ft)
		{
			Section_ft->SetValue(*ConfigMappings[static_cast<int>(InConfig)].Key, InValue);
		}

		ft.Save(*Path);
	}

	template<typename T>
	T GetEngineConfigValue(EEngineConfigValueType InConfig) const
	{
		auto Section = FindSection(InConfig);
		if (Section == EEngineConfigSectionType::ECS_None)
		{
			return T();
		}

		if(!EngineConfig.Contains(Section))
		{
			return T();
		}

		if (!EngineConfig[Section].Contains(InConfig))
		{
			return T();
		}

		const FString& Value = EngineConfig[Section][InConfig];

		if (std::is_same_v<T, int>)
		{
			return std::stoi(*Value);
		}
		else if (std::is_same_v<T, float>)
		{
			return std::stof(*Value);
		}
		/*else if (std::is_same_v<T, FString>)
		{
			return Value;
		}*/
		else
			return T();
	}

	//void SaveEngineConfig(EEngineConfigValueType InConfig, UINT InValue);
	//void SaveEngineConfig(EEngineConfigValueType InConfig, float InValue);
	//void SaveEngineConfig(EEngineConfigValueType InConfig, const FString& InValue);

	void SaveAllConfig();

private:
	EEngineConfigSectionType FindSection(const EEngineConfigValueType& InValueType) const;
	FString GetPath();
private:
	TMap<EEngineConfigSectionType, TMap<EEngineConfigValueType, FString>> EngineConfig;

	INI::File ft;
	FString Path;
};