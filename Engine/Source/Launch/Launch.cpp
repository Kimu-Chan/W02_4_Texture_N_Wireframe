#include "pch.h" 
#include "Engine/Engine.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
        UNREFERENCED_PARAMETER(hPrevInstance);
        UNREFERENCED_PARAMETER(lpCmdLine);
        UNREFERENCED_PARAMETER(nShowCmd);


        UEngine& Engine = UEngine::Get();
        
        Engine.LoadEngineConfig();

        Engine.Initialize(
            hInstance, 
            L"Jungle Engine", 
            L"JungleWindow", 
            UEngine::Get().GetEngineConfigValue(EEngineConfig::EEC_ScreenWidth), 
            UEngine::Get().GetEngineConfigValue(EEngineConfig::EEC_ScreenWidth)
        );

        Engine.Run();

        Engine.Shutdown();

        return 0;
}
