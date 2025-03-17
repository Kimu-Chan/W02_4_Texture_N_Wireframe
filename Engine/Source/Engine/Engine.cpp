#include "pch.h" 
#include "Engine.h"

#include "WorldGrid.h"
#include "Static/EditorManager.h"
#include "Core/Input/PlayerInput.h"
#include "Core/Input/PlayerController.h"
#include "CoreUObject/ObjectFactory.h"
#include "CoreUObject/World.h"
#include "Gizmo/Axis.h"
#include "GameFrameWork/Camera.h"

#ifdef _DEBUG
#pragma comment(lib, "DirectXTK/Libs/x64/Debug/DirectXTK.lib")
#else
#pragma comment(lib, "DirectXTK/Libs/x64/Release/DirectXTK.lib")
#endif
#include "GameFrameWork/Sphere.h"
#include "Core/Rendering/TextureLoader.h"


class AArrow;
class APicker;

// ImGui WndProc
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT UEngine::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    // Handle ImGui Msg
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
    {
        return true;
    }
    
    switch (uMsg)
    {
    case WM_DESTROY:    // Window Close, Alt + F4
        PostQuitMessage(0);
        return 0;

    // Begin Handle Input
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MOUSEWHEEL:
        DirectX::Mouse::ProcessMessage(uMsg, wParam, lParam);
        break;

    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
        DirectX::Keyboard::ProcessMessage(uMsg, wParam, lParam);
        break;
    // End Handle Input
    
    case WM_SIZE:
        {
            // 다른 case에서 아래의 변수에 접근하지 못하도록 스코프 제한
            if (wParam == SIZE_MINIMIZED)
            {
                return 0;
            }
            int32 Width = static_cast<int32>(LOWORD(lParam));
            int32 Height = static_cast<int32>(HIWORD(lParam));
            UEngine::Get().ResizeWidth = Width;
            UEngine::Get().ResizeHeight = Height;
            UEngine::Get().UpdateWindowSize(Width, Height);
        }
        return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void UEngine::Initialize(HINSTANCE hInstance, const WCHAR* InWindowTitle, const WCHAR* InWindowClassName, int InScreenWidth, int InScreenHeight, EScreenMode InScreenMode)
{
    // ini파일 로드
	EngineConfig = new FEngineConfig();
	EngineConfig->LoadEngineConfig();

	int width = EngineConfig->GetEngineConfigValue<int>(EEngineConfigValueType::EEC_ScreenWidth);
	int height = EngineConfig->GetEngineConfigValue<int>(EEngineConfigValueType::EEC_ScreenHeight);

    WindowInstance = hInstance;
    WindowTitle = InWindowTitle;
    WindowClassName = InWindowClassName;
    ScreenWidth = width <= 0 ? InScreenWidth : width;
    ScreenHeight = height <= 0 ? InScreenHeight : height;

    ScreenMode = InScreenMode;

    InitWindow(ScreenWidth, ScreenHeight);

	EngineConfig->SaveEngineConfig<int>(EEngineConfigValueType::EEC_ScreenWidth, ScreenWidth);
	EngineConfig->SaveEngineConfig<int>(EEngineConfigValueType::EEC_ScreenHeight, ScreenHeight);

	// Get Client Rect
	RECT ClientRect;
	GetClientRect(WindowHandle, &ClientRect);
	ScreenWidth = ClientRect.right - ClientRect.left;
	ScreenHeight = ClientRect.bottom - ClientRect.top;

    APlayerInput::Get().SetWindowSize(ScreenWidth, ScreenHeight);

    InitRenderer();

    
    InitTextureLoader();

    InitializedScreenWidth = ScreenWidth;
    InitializedScreenHeight = ScreenHeight;
    InitWorld();
    ui.Initialize(WindowHandle, *Renderer, ScreenWidth, ScreenHeight);
    
    UE_LOG("Engine Initialized!");
}

void UEngine::Run()
{
    // Limit FPS
    constexpr int TargetFPS = 60;
    constexpr double TargetDeltaTime = 1000.0f / TargetFPS; // 1 FPS's target time (ms)

    LARGE_INTEGER Frequency;
    QueryPerformanceFrequency(&Frequency);

    LARGE_INTEGER StartTime;
    QueryPerformanceCounter(&StartTime);

    bIsExit = true;
    while (bIsExit)
    {
        // DeltaTime //
        const LARGE_INTEGER EndTime = StartTime;
        QueryPerformanceCounter(&StartTime);

        const float DeltaTime = static_cast<float>(StartTime.QuadPart - EndTime.QuadPart) / static_cast<float>(Frequency.QuadPart);

		// Message Loop //
        MSG Msg;
        while (PeekMessage(&Msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&Msg);
            DispatchMessage(&Msg);

            if (Msg.message == WM_QUIT)
            {
                bIsExit = false;
                break;
            }
        }

        // Handle window being minimized or screen locked
        if (Renderer->IsOccluded()) continue;

        // Renderer Update
        Renderer->PrepareRender();
        Renderer->PrepareShader();

        // World Update
        if (World)
        {
            World->Tick(DeltaTime);
            World->Render(DeltaTime);
            World->LateTick(DeltaTime);
        }
        
        // ui Update
        ui.Update();

        // UI입력을 우선으로 처리
        APlayerInput::Get().UpdateInput();
        APlayerController::Get().ProcessPlayerInput(DeltaTime);

        Renderer->SwapBuffer();

        // FPS 제한
        double ElapsedTime;
        do
        {
            Sleep(0);

            LARGE_INTEGER CurrentTime;
            QueryPerformanceCounter(&CurrentTime);

            ElapsedTime = static_cast<double>(CurrentTime.QuadPart - StartTime.QuadPart) * 1000.0 / static_cast<double>(Frequency.QuadPart);
        } while (ElapsedTime < TargetDeltaTime);
    }
}


void UEngine::Shutdown()
{
    ShutdownWindow();
}


void UEngine::InitWindow(int InScreenWidth, int InScreenHeight)
{
	// Register Window Class //
    WNDCLASSW wnd_class{};
    wnd_class.lpfnWndProc = WndProc;
    wnd_class.hInstance = WindowInstance;
    wnd_class.lpszClassName = WindowClassName;
    RegisterClassW(&wnd_class);

    // Create Window Handle //
    WindowHandle = CreateWindowExW(
        WS_EX_NOREDIRECTIONBITMAP,
        WindowClassName, WindowTitle,
        WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        ScreenWidth, ScreenHeight,
        nullptr, nullptr, WindowInstance, nullptr
    );

	//@TODO: Implement Fullscreen, Borderless Mode
    if (ScreenMode != EScreenMode::Windowed)
    {
        std::cout << "not implement Fullscreen and Borderless mode." << '\n';
    }

    // Focus Window //
    ShowWindow(WindowHandle, SW_SHOW);
    SetForegroundWindow(WindowHandle);
    SetFocus(WindowHandle);
}

void UEngine::InitRenderer()
{
    Renderer = std::make_unique<URenderer>();
    Renderer->Create(WindowHandle);
    Renderer->CreateShader();
    Renderer->CreateConstantBuffer();

    Renderer->GenerateWorldGridVertices(WorldGridCellPerSide);
}

void UEngine::InitWorld()
{
    World = FObjectFactory::ConstructObject<UWorld>();

    if (ACamera* Camera = World->SpawnActor<ACamera>())
    {
        FEditorManager::Get().SetCamera(Camera);

        // 렌더러가 먼저 초기화 되므로, 카메라가 생성되는 시점인 현재 함수에서 프로젝션 매트릭스 업데이트
        Renderer->UpdateProjectionMatrix(Camera);

        // 카메라 ini 읽어오기
		float PosX = EngineConfig->GetEngineConfigValue<float>(EEngineConfigValueType::EEC_EditorCameraPosX, -5.f);
		float PosY = EngineConfig->GetEngineConfigValue<float>(EEngineConfigValueType::EEC_EditorCameraPosY);
		float PosZ = EngineConfig->GetEngineConfigValue<float>(EEngineConfigValueType::EEC_EditorCameraPosZ);

		FVector CameraPos = FVector(PosX, PosY, PosZ);
		float RotX = EngineConfig->GetEngineConfigValue<float>(EEngineConfigValueType::EEC_EditorCameraRotX);
		float RotY = EngineConfig->GetEngineConfigValue<float>(EEngineConfigValueType::EEC_EditorCameraRotY);
		float RotZ = EngineConfig->GetEngineConfigValue<float>(EEngineConfigValueType::EEC_EditorCameraRotZ);
		float RotW = EngineConfig->GetEngineConfigValue<float>(EEngineConfigValueType::EEC_EditorCameraRotW, 1.f);

		FQuat CameraRot = FQuat(RotX, RotY, RotZ, RotW);
		FTransform CameraTransform = FTransform(CameraPos, CameraRot, FVector(1,1,1));
		Camera->SetActorTransform(CameraTransform);

		float CameraSpeed = EngineConfig->GetEngineConfigValue<float>(EEngineConfigValueType::EEC_EditorCameraSpeed, 1.f);
        APlayerController::Get().SetCurrentSpeed(CameraSpeed);
        Renderer->UpdateViewMatrix(CameraTransform);

        float CameraSensitivity = EngineConfig->GetEngineConfigValue<float>(EEngineConfigValueType::EEC_EditorCameraSensitivity, 10.f);
        APlayerController::Get().SetMouseSensitivity(CameraSensitivity);
    }

    //// Test
    //AArrow* Arrow = World->SpawnActor<AArrow>();
    //World->SpawnActor<ASphere>();
    
    World->SpawnActor<AAxis>();
    World->SpawnActor<APicker>();
    WorldGrid = World->SpawnActor<AWorldGrid>();

    World->BeginPlay();
}

void UEngine::InitTextureLoader()
{
    // TextureLoader 생성
    TextureLoaderInstance = new TextureLoader(Renderer->GetDevice(), Renderer->GetDeviceContext());

	// Texture Load
    LoadTexture(L"ASCII", L"ASCII.png", 16, 16);
    LoadTexture(L"Cat", L"Cat.jpg", 1, 1);

	const TextureInfo* TextureInfo = GetTextureInfo(L"ASCII");

    int b = 0;
    
}

void UEngine::ShutdownWindow()
{
    DestroyWindow(WindowHandle);
    WindowHandle = nullptr;

    UnregisterClassW(WindowClassName, WindowInstance);
    WindowInstance = nullptr;

    ui.Shutdown();
    EngineConfig->SaveAllConfig();
	delete EngineConfig;
}

void UEngine::UpdateWindowSize(const uint32 InScreenWidth, const uint32 InScreenHeight)
{
    ScreenWidth = InScreenWidth;
    ScreenHeight = InScreenHeight;

    if(Renderer)
    {
        Renderer->OnUpdateWindowSize(InScreenWidth, InScreenHeight);
    }

    if (ui.bIsInitialized)
    {
        ui.OnUpdateWindowSize(InScreenWidth, InScreenHeight);
    }
	ResizeWidth = ResizeHeight = 0;

    APlayerInput::Get().SetWindowSize(ScreenWidth, ScreenHeight);

	EngineConfig->SaveEngineConfig<int>(EEngineConfigValueType::EEC_ScreenWidth, ScreenWidth);
	EngineConfig->SaveEngineConfig<int>(EEngineConfigValueType::EEC_ScreenHeight, ScreenHeight);
}

UObject* UEngine::GetObjectByUUID(uint32 InUUID) const
{
    if (const auto Obj = GObjects.Find(InUUID))
    {
        return Obj->get();
    }
    return nullptr;
}

bool UEngine::LoadTexture(const std::wstring& Name, const std::wstring& FileName, int32 Rows, int32 Columns)
{
	if (TextureLoaderInstance)
	{
		return TextureLoaderInstance->LoadTexture(Name, FileName, Rows, Columns);
	}
    return false;
}

const TextureInfo* UEngine::GetTextureInfo(const std::wstring& Name) const
{
    if (TextureLoaderInstance)
    {
		return TextureLoaderInstance->GetTextureInfo(Name);
    }
    return nullptr;
}
