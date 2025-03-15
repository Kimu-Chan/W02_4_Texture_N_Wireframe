#pragma once
#define DEFAULT_INI_FILE_PATH L"engine.ini"

enum class EEngineConfig 
{
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

struct ConfigMapping
{
	EEngineConfig Config;
	const WCHAR* Key;
};

static const ConfigMapping ConfigMappings[] = {
	{ EEngineConfig::EEC_ScreenWidth, L"ScreenWidth" },
	{ EEngineConfig::EEC_ScreenHeight, L"ScreenHeight" },

	{ EEngineConfig::EEC_EditorCameraPosX, L"EditorCameraPosX" },
	{ EEngineConfig::EEC_EditorCameraPosY, L"EditorCameraPosY" },
	{ EEngineConfig::EEC_EditorCameraPosZ, L"EditorCameraPosZ" },

	{ EEngineConfig::EEC_EditorCameraRotX, L"EditorCameraRotX" },
	{ EEngineConfig::EEC_EditorCameraRotY, L"EditorCameraRotY" },
	{ EEngineConfig::EEC_EditorCameraRotZ, L"EditorCameraRotZ" },

	{ EEngineConfig::EEC_EditorCameraSpeed, L"EditorCameraSpeed" }
	// !TODO : EngineConfig 추가시 추가
};