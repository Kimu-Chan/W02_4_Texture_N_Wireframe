#include "pch.h" 
#include "Engine/Engine.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
        UNREFERENCED_PARAMETER(hPrevInstance);
        UNREFERENCED_PARAMETER(lpCmdLine);
        UNREFERENCED_PARAMETER(nShowCmd);


        UEngine& Engine = UEngine::Get();
        Engine.Initialize(hInstance, L"Jungle Engine", L"JungleWindow", 1920, 1080);

        Engine.Run();

        Engine.Shutdown();

        return 0;
}
