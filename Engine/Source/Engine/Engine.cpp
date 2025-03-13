#include "pch.h" 
#include "Engine.h"
#include "Static/EditorManager.h"
#include "Core/Input/PlayerInput.h"
#include "Core/Input/PlayerController.h"
#include "CoreUObject/ObjectFactory.h"
#include "CoreUObject/World.h"
#include "Gizmo/Axis.h"
#include "Debugging/DebugConsole.h"
#include "GameFrameWork/Camera.h"
#include "GameFrameWork/Sphere.h"

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
        break;

    // Handle Key Input
	case WM_KEYDOWN:    //@TODO: WinApi�� AysncKeyState�� ��ü ����
        APlayerInput::Get().KeyDown(static_cast<EKeyCode>(wParam));
        if ((lParam>>30)%2 != 0)
        {
            APlayerInput::Get().KeyOnceUp(static_cast<EKeyCode>( wParam ));
        }
        break;
    case WM_KEYUP:
        APlayerInput::Get().KeyUp(static_cast<EKeyCode>( wParam ));
        break;
    case WM_LBUTTONDOWN:
        APlayerInput::Get().HandleMouseInput(hWnd, lParam, true, false);
        break;
    case WM_LBUTTONUP:
        APlayerInput::Get().HandleMouseInput(hWnd, lParam, false, false);
        break;
    case WM_RBUTTONDOWN:
        APlayerInput::Get().HandleMouseInput(hWnd, lParam, true, true);
        break;
    case WM_RBUTTONUP:
        APlayerInput::Get().HandleMouseInput(hWnd, lParam, false, true);
        break;
	case WM_MOUSEWHEEL:
		APlayerInput::Get().HandleMouseWheel(wParam);
		break;

    case WM_SIZE:
                UEngine::Get().UpdateWindowSize(LOWORD(lParam), HIWORD(lParam));
                break;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

void UEngine::Initialize(HINSTANCE hInstance, const WCHAR* InWindowTitle, const WCHAR* InWindowClassName, int InScreenWidth, int InScreenHeight, EScreenMode InScreenMode)
{
    WindowInstance = hInstance;
    WindowTitle = InWindowTitle;
    WindowClassName = InWindowClassName;
    ScreenMode = InScreenMode;
    ScreenWidth = InScreenWidth;
    ScreenHeight = InScreenHeight;

    InitWindow(InScreenWidth, InScreenWidth);

    InitRenderer();

    InitWorld();

    InitializedScreenWidth = ScreenWidth;
    InitializedScreenHeight = ScreenHeight;
    
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

		// Input PreProcess //
        APlayerInput::Get().PreProcessInput();
        
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

        // Renderer Update
        Renderer->Prepare();
        Renderer->PrepareShader();

        // World Update
        if (World)
        {
            World->Tick(DeltaTime);
            World->Render();
            World->LateTick(DeltaTime);
        }

        //�� Actor���� TickActor() -> PlayerTick() -> TickPlayerInput() ȣ���ϴµ� ������ Message���� ó���ϰ� �ִ�.
        APlayerInput::Get().TickPlayerInput();  //@TODO: TickPlayerInput�� �ű���.
        
        // TickPlayerInput
        APlayerController::Get().ProcessPlayerInput(DeltaTime);
        
        // ui Update
        ui.Update();

        Renderer->SwapBuffer();

        // FPS ����
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
        0, WindowClassName, WindowTitle,
        WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        InScreenWidth, InScreenHeight,
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
}

void UEngine::InitWorld()
{
    World = FObjectFactory::ConstructObject<UWorld>();

    FEditorManager::Get().SetCamera(World->SpawnActor<ACamera>());

    //// Test
    //AArrow* Arrow = World->SpawnActor<AArrow>();
    //World->SpawnActor<ASphere>();
    
    World->SpawnActor<AAxis>();
    World->SpawnActor<APicker>();

    World->BeginPlay();
}

void UEngine::ShutdownWindow()
{
    DestroyWindow(WindowHandle);
    WindowHandle = nullptr;

    UnregisterClassW(WindowClassName, WindowInstance);
    WindowInstance = nullptr;

        ui.Shutdown();
}

void UEngine::UpdateWindowSize(UINT InScreenWidth, UINT InScreenHeight)
{
    ScreenWidth = InScreenWidth;
    ScreenHeight = InScreenHeight;

    if(Renderer)
    {
        Renderer->OnUpdateWindowSize(ScreenWidth, ScreenHeight);
    }

    if (ui.bIsInitialized)
    {
        ui.OnUpdateWindowSize(ScreenWidth, ScreenHeight);
    }
}

UObject* UEngine::GetObjectByUUID(uint32 InUUID) const
{
    if (const auto Obj = GObjects.Find(InUUID))
    {
        return Obj->get();
    }
    return nullptr;
}
