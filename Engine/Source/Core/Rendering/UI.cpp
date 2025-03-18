#include "pch.h" 
#include "UI.h"

#include "Axis.h"
#include "Core/HAL/PlatformMemory.h"
#include "URenderer.h"
#include "Debugging/DebugConsole.h"
#include "Static/EditorManager.h"
#include "CoreUObject/World.h"
#include "CoreUObject/Components/PrimitiveComponent.h"
#include "Editor/EditorDesigner.h"
#include "Editor/Font/IconDefs.h"
#include "Editor/Font/RawFonts.h"
#include "Editor/Windows/ConsoleWindow.h"
#include "Engine/GameFrameWork/Actor.h"
#include "Engine/GameFrameWork/Camera.h"
#include "Engine/GameFrameWork/Sphere.h"
#include "Engine/GameFrameWork/Cube.h"
#include "Engine/GameFrameWork/Arrow.h"
#include "Engine/GameFrameWork/Cone.h"
#include "Engine/GameFrameWork/Cylinder.h"
#include "Gizmo/GizmoHandle.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_internal.h"
#include "Input/PlayerController.h"
#include "Input/PlayerInput.h"

//@TODO: Replace with EditorWindow

std::shared_ptr<ConsoleWindow> UI::ConsoleWindowInstance = nullptr;

void UI::Initialize(HWND hWnd, const URenderer& Renderer, uint32 ScreenWidth, uint32 ScreenHeight)
{
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	//CreateUsingFont();

    // Fix Font Size
    io.FontGlobalScale = 1.0f;

    // Initialize ImGui Backend
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(Renderer.GetDevice(), Renderer.GetDeviceContext());

    ScreenSize = ImVec2(static_cast<float>(ScreenWidth), static_cast<float>(ScreenHeight));
    InitialScreenSize = ScreenSize;
    bIsInitialized = true;
    
    io.DisplaySize = ScreenSize;

    PreRatio = GetRatio();
    CurRatio = GetRatio();

    // Add Windows
    //@TODO: Control, Property, Stat, etc...
    ConsoleWindowInstance = std::make_shared<ConsoleWindow>();
	UEditorDesigner::Get().AddWindow("ConsoleWindow", ConsoleWindowInstance);
}

void UI::Update()
{
	// Set ImGui Style //
    PreferenceStyle();

	// New Frame //
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // 화면비 갱신 //
    if (bWasWindowSizeUpdated)
    {
        PreRatio = CurRatio;
        CurRatio = GetRatio();
        UE_LOG("Current Ratio: %f, %f", CurRatio.x, CurRatio.y);
    }

    if (bShowDemoWindow)
		ImGui::ShowDemoWindow(&bShowDemoWindow);

    RenderControlPanelWindow();
    RenderPropertyWindow();
    Debug::ShowConsole(bWasWindowSizeUpdated, PreRatio, CurRatio);
    RenderSceneManagerWindow();


    // UI::RenderSomePanel 들에 대한 업데이트 완료 //
    bWasWindowSizeUpdated = false;

	// Render Windows //
	UEditorDesigner::Get().Render();

	// Render ImGui //
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    bool bUiInput = ImGui::IsAnyItemHovered() || ImGui::IsAnyItemActive() || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
    APlayerController::Get().SetIsUiInput(bUiInput);
}


void UI::Shutdown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void UI::OnUpdateWindowSize(UINT InScreenWidth, UINT InScreenHeight)
{
	// Create ImGUI Resources Again
    ImGui_ImplDX11_InvalidateDeviceObjects();
    ImGui_ImplDX11_CreateDeviceObjects();

	// Resize ImGui Window
    ScreenSize = ImVec2(static_cast<float>(InScreenWidth), static_cast<float>(InScreenHeight));

    bWasWindowSizeUpdated = true;

    // Render Windows //
    UEditorDesigner::Get().OnResize(InScreenWidth, InScreenHeight);
}

void UI::RenderControlPanelWindow()
{
    ImGui::Begin("Jungle Control Panel");

    if (bWasWindowSizeUpdated)
    {
        auto* Window = ImGui::GetCurrentWindow();

        ImGui::SetWindowPos(ResizeToScreen(Window->Pos));
        ImGui::SetWindowSize(ResizeToScreen(Window->Size));
    }
    
    ImGui::Text("Hello, Jungle World!");

    ImGui::Separator();

    ImGui::Text("Mouse pos: (%g, %g)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
	ImGui::Text("Screen Size: (%g, %g)", ScreenSize.x, ScreenSize.y);

    ImGui::Separator();

    ImGui::Text("FPS: %.3f (what is that ms)", ImGui::GetIO().Framerate);
    RenderMemoryUsage();
    
    ImGui::Separator();

    RenderPrimitiveSelection();

    ImGui::Separator();

    RenderCameraSettings();

    ImGui::Separator();

    RenderRenderMode();

	ImGui::Separator();

    RenderGridGap();

    ImGui::Separator();

    RenderDebugRaycast();

	ImGui::Separator();

    if (ImGui::Button("Toggle New Console"))
    {
        UEditorDesigner::Get().Toggle();
    }
	ImGui::SameLine();
    ImGui::Checkbox("Demo Window", &bShowDemoWindow);

    ImGui::End();
}

void UI::RenderMemoryUsage()
{
    const uint64 ContainerAllocByte = FPlatformMemory::GetAllocationBytes<EAT_Container>();
    const uint64 ContainerAllocCount = FPlatformMemory::GetAllocationCount<EAT_Container>();
    const uint64 ObjectAllocByte = FPlatformMemory::GetAllocationBytes<EAT_Object>();
    const uint64 ObjectAllocCount = FPlatformMemory::GetAllocationCount<EAT_Object>();
    ImGui::Text(
        "Container Memory Uses: %llubyte, Count: %llu",
        ContainerAllocByte,
        ContainerAllocCount
    );
    ImGui::Text(
        "Object Memory Uses: %llubyte, Count: %llu Objects",
        ObjectAllocByte,
        ObjectAllocCount
    );
    ImGui::Text(
        "Total Memory Uses: %llubyte, Count: %llu",
        ContainerAllocByte + ObjectAllocByte,
        ContainerAllocCount + ObjectAllocCount
    );
}

void UI::RenderPrimitiveSelection()
{
    const char* items[] = { "Sphere", "Cube", "Cylinder", "Cone", "Arrow"};

    ImGui::Combo("Primitive", &currentItem, items, IM_ARRAYSIZE(items));

    if (ImGui::Button("Spawn"))
    {
        UWorld* World = UEngine::Get().GetWorld();
        for (int i = 0 ;  i < NumOfSpawn; i++)
        {
            if (strcmp(items[currentItem], "Sphere") == 0)
            {
                World->SpawnActor<ASphere>();
            }
            else if (strcmp(items[currentItem], "Cube") == 0)
            {
                World->SpawnActor<ACube>();
            }
            else if (strcmp(items[currentItem], "Cylinder") == 0)
            {
                World->SpawnActor<ACylinder>();
            }
            else if (strcmp(items[currentItem], "Cone") == 0)
            {
                World->SpawnActor<ACone>();
            }
            else if (strcmp(items[currentItem], "Arrow") == 0)
            {
                World->SpawnActor<AArrow>();
            }
            //else if (strcmp(items[currentItem], "Triangle") == 0)
            //{
            //    Actor->AddComponent<UTriangleComp>();   
            //}
        }
    }
    ImGui::SameLine();
    ImGui::InputInt("Number of spawn", &NumOfSpawn, 0);

    ImGui::Separator();

    UWorld* World = UEngine::Get().GetWorld();
    uint32 bufferSize = 100;
    char* SceneNameInput = new char[bufferSize];
    strcpy_s(SceneNameInput, bufferSize, *World->SceneName);
    
    if (ImGui::InputText("Scene Name", SceneNameInput, bufferSize))
    {
    	World->SceneName = SceneNameInput;
    }
    
    if (ImGui::Button("New Scene"))
    {
        World->ClearWorld();
    }
    ImGui::SameLine();
    if (ImGui::Button("Save Scene"))
    {
        World->SaveWorld();   
    }
    ImGui::SameLine();
    if (ImGui::Button("Load Scene"))
    {
        World->LoadWorld(SceneNameInput);
    }
}

void UI::RenderCameraSettings()
{
    ImGui::Text("Camera");

    ACamera* Camera = FEditorManager::Get().GetCamera();

    bool IsOrthogonal;
    if (Camera->ProjectionMode == ECameraProjectionMode::Orthographic)
    {
        IsOrthogonal = true;
    }
    else if (Camera->ProjectionMode == ECameraProjectionMode::Perspective)
    {
        IsOrthogonal = false;
    }

    if (ImGui::Checkbox("Orthogonal", &IsOrthogonal))
    {
        if (IsOrthogonal)
        {
            Camera->ProjectionMode = ECameraProjectionMode::Orthographic;
        }
        else
        {
            Camera->ProjectionMode = ECameraProjectionMode::Perspective;
        }

        UEngine::Get().GetRenderer()->UpdateProjectionMatrix(Camera);
    }

    float FOV = Camera->GetFieldOfView();
    if (ImGui::DragFloat("FOV", &FOV, 0.1f, 20.f, 150.f))
    {
        FOV = FMath::Clamp(FOV, 20.f, 150.f);
        Camera->SetFieldOfView(FOV);

        UEngine::Get().GetRenderer()->UpdateProjectionMatrix(Camera);
    }

    float NearFar[2] = { Camera->GetNearClip(), Camera->GetFarClip() };
    if (ImGui::DragFloat2("Near clip, Far clip", NearFar, 0.1f, 0.01f, 200.f))
    {
        NearFar[0] = FMath::Clamp(NearFar[0], 0.01f, 200.f);
        NearFar[1] = FMath::Clamp(NearFar[1], 0.01f, 200.f);

        if (NearFar[0] > NearFar[1])
        {
            std::swap(NearFar[0], NearFar[1]);
        }

        Camera->SetNear(NearFar[0]);
        Camera->SetFar(NearFar[1]);
        
        UEngine::Get().GetRenderer()->UpdateProjectionMatrix(Camera);
    }
    
    FVector CameraPosition = Camera->GetActorTransform().GetPosition();
    if (ImGui::DragFloat3("Camera Location", reinterpret_cast<float*>(&CameraPosition), 0.1f))
    {
        FTransform Trans = Camera->GetActorTransform();
        Trans.SetPosition(CameraPosition);
        Camera->SetActorTransform(Trans);
    }

    FVector PrevEulerAngle = Camera->GetActorTransform().GetRotation().GetEuler();
    FVector UIEulerAngle = { PrevEulerAngle.X, PrevEulerAngle.Y, PrevEulerAngle.Z };
    if (ImGui::DragFloat3("Camera Rotation", reinterpret_cast<float*>(&UIEulerAngle), 0.1f))
    {
        FTransform Transform = Camera->GetActorTransform();

        //FVector DeltaEulerAngle = UIEulerAngle - PrevEulerAngle;
        //Transform.Rotate(DeltaEulerAngle);
        
        UIEulerAngle.Y = FMath::Clamp(UIEulerAngle.Y, -Camera->MaxYDegree, Camera->MaxYDegree);
        Transform.SetRotation(UIEulerAngle);
        Camera->SetActorTransform(Transform);
    }

    float CurrentSpeed = APlayerController::Get().GetCurrentSpeed();
    const float CameraMaxSpeed = APlayerController::Get().GetMaxSpeed();
    const float CameraMinSpeed = APlayerController::Get().GetMinSpeed();
    if (ImGui::DragFloat("Camera Speed", &CurrentSpeed, 0.1f, CameraMinSpeed, CameraMaxSpeed))
    {
        APlayerController::Get().SetCurrentSpeed(CurrentSpeed);
    }

    float CurrentSensitivity = APlayerController::Get().GetMouseSensitivity();
    const float CameraMaxSensitivity = APlayerController::Get().GetMaxSensitivity();
    const float CameraMinSensitivity = APlayerController::Get().GetMinSensitivity();
    if (ImGui::DragFloat("Camera Sensitivity", &CurrentSensitivity, 0.1f, CameraMinSensitivity, CameraMaxSensitivity))
    {
        APlayerController::Get().SetMouseSensitivity(CurrentSensitivity);
    }

    FVector Forward = Camera->GetActorTransform().GetForward();
    FVector Up = Camera->GetActorTransform().GetUp();
    FVector Right = Camera->GetActorTransform().GetRight();

    ImGui::Text("Camera GetForward(): (%.2f %.2f %.2f)", Forward.X, Forward.Y, Forward.Z);
    ImGui::Text("Camera GetUp(): (%.2f %.2f %.2f)", Up.X, Up.Y, Up.Z);
    ImGui::Text("Camera GetRight(): (%.2f %.2f %.2f)", Right.X, Right.Y, Right.Z);

    ImGui::Separator();
}

void UI::RenderRenderMode()
{
	const char* items[] = { "Solid", "Wireframe" };
	ImGui::Combo("Render Mode", &currentItem, items, IM_ARRAYSIZE(items));
	if (ImGui::Button("Apply"))
	{
		URenderer* Renderer = UEngine::Get().GetRenderer();
		if (currentItem == 0)
		{
			Renderer->SetRenderMode(EViewModeIndex::ERS_Solid);
		}
		else if (currentItem == 1)
		{
			Renderer->SetRenderMode(EViewModeIndex::ERS_Wireframe);
		}
	}

    bool bShowPrimitives = UEngine::Get().GetShowPrimitives();
    if (ImGui::Checkbox("Show Primitives", &bShowPrimitives))
    {
        UEngine::Get().SetShowPrimitives(bShowPrimitives);
    }
    
    ImGui::Separator();
}

void UI::RenderPropertyWindow()
{
    ImGui::Begin("Properties");

    if (bWasWindowSizeUpdated)
    {
        auto* Window = ImGui::GetCurrentWindow();

        ImGui::SetWindowPos(ResizeToScreen(Window->Pos));
        ImGui::SetWindowSize(ResizeToScreen(Window->Size));
    }
    USceneComponent* SelectedComponent = FEditorManager::Get().GetSelectedComponent();
    if (SelectedComponent != nullptr)
    {
        ImGui::Text("Selected Actor : %s", *SelectedComponent->GetOwner()->GetName().ToString());
		ImGui::Text("Selected Component : %s", *SelectedComponent->GetName().ToString());

		bool bIsLocal = FEditorManager::Get().GetGizmoHandle()->bIsLocal;
		if (ImGui::Checkbox("Local", &bIsLocal))
		{
			FEditorManager::Get().ToggleGizmoHandleLocal(bIsLocal);
		}

        FTransform selectedTransform = SelectedComponent->GetComponentTransform();
        float position[] = { selectedTransform.GetPosition().X, selectedTransform.GetPosition().Y, selectedTransform.GetPosition().Z };
        float scale[] = { selectedTransform.GetScale().X, selectedTransform.GetScale().Y, selectedTransform.GetScale().Z };

        if (ImGui::DragFloat3("Translation", position, 0.1f))
        {
            selectedTransform.SetPosition(position[0], position[1], position[2]);
            SelectedComponent->SetRelativeTransform(selectedTransform);
        }

        FVector PrevEulerAngle = selectedTransform.GetRotation().GetEuler();
        FVector UIEulerAngle = PrevEulerAngle;
        if (ImGui::DragFloat3("Rotation", reinterpret_cast<float*>(&UIEulerAngle), 0.1f))
        {
            FVector DeltaEulerAngle = UIEulerAngle - PrevEulerAngle;
			UE_LOG("DeltaEulerAngle %f, %f, %f", DeltaEulerAngle.X, DeltaEulerAngle.Y, DeltaEulerAngle.Z);

            selectedTransform.Rotate(DeltaEulerAngle);
            SelectedComponent->SetRelativeTransform(selectedTransform);
        }
        if (ImGui::DragFloat3("Scale", scale, 0.1f))
        {
            selectedTransform.SetScale(scale[0], scale[1], scale[2]);
            SelectedComponent->SetRelativeTransform(selectedTransform);
        }
        if (FEditorManager::Get().GetGizmoHandle() != nullptr)
        {
            AGizmoHandle* Gizmo = FEditorManager::Get().GetGizmoHandle();
            if(Gizmo->GetGizmoType() == EGizmoType::Translate)
            {
                    ImGui::Text("GizmoType: Translate");
            }
            else if (Gizmo->GetGizmoType() == EGizmoType::Rotate)
            {
                    ImGui::Text("GizmoType: Rotate");
            }
            else if (Gizmo->GetGizmoType() == EGizmoType::Scale)
            {
                    ImGui::Text("GizmoType: Scale");
            }
        }
    }
    ImGui::End();
}

void UI::RenderGridGap()
{
    ImGui::Text("World Grid");

    float MaxVal = 10.f;
    float MinVal = 0.5f;

    float GridGap = UEngine::Get().GetWorldGridGap();

    if (ImGui::DragFloat("Grid Gap", &GridGap, 0.01f, MinVal, MaxVal))
    {
        GridGap = GridGap > MaxVal ? MaxVal : (GridGap < MinVal ? MinVal : GridGap); // Clamp
        UEngine::Get().SetWorldGridGap(GridGap);
    }

    ImGui::Separator();
}

void UI::RenderDebugRaycast()
{
    bool bDebugRaycast = UEngine::Get().GetWorld()->IsDebuggingRaycast();
    if (ImGui::Checkbox("Debug Raycast", &bDebugRaycast))
    {
        UEngine::Get().GetWorld()->SetDebugRaycast(bDebugRaycast);
    }
}

void UI::RenderSceneManagerWindow()
{
    ImGui::Begin("Scene Manager");

    TArray<AActor*> Actors = UEngine::Get().GetWorld()->GetActors();
    
    if (ImGui::TreeNodeEx("Primitives", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (const auto& Actor : Actors)
        {
            if (Actor->IsGizmoActor() || Actor->IsA<AAxis>())
            {
                continue;
            }
            
            TSet<UActorComponent*> Comps = Actor->GetComponents();
            
            bool bHasPrimitive = false;
            for (const auto& Comp : Comps)
            {
                if (Comp->IsA<UPrimitiveComponent>())
                {
                    bHasPrimitive = true;
                    break;
                }
            }
            
            if (bHasPrimitive)
            {
                ImGui::Text(*Actor->GetName().ToString());
            }
        }
        
        ImGui::TreePop();
    }
    ImGui::End();
}

void UI::PreferenceStyle()
{
    // Window
    ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.9f);
    ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.5f, 0.0f, 1.0f);
    ImGui::GetStyle().Colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    ImGui::GetStyle().WindowRounding = 5.0f;

    ImGui::GetStyle().FrameRounding = 3.0f;

    // Sep
    ImGui::GetStyle().Colors[ImGuiCol_Separator] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);

    // Frame
    ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = ImVec4(0.31f, 0.31f, 0.31f, 0.6f);
    ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive] = ImVec4(0.203f, 0.203f, 0.203f, 0.6f);
    ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.0f, 0.5f, 0.0f, 0.6f);

    // Button
    ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(0.105f, 0.105f, 0.105f, 0.6f);
    ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = ImVec4(0.105f, 0.105f, 0.105f, 0.6f);
    ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = ImVec4(0.0f, 0.5f, 0.0f, 0.6f);

    ImGui::GetStyle().Colors[ImGuiCol_Header] = ImVec4(0.203f, 0.203f, 0.203f, 0.6f);
    ImGui::GetStyle().Colors[ImGuiCol_HeaderActive] = ImVec4(0.105f, 0.105f, 0.105f, 0.6f);
    ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered] = ImVec4(0.0f, 0.5f, 0.0f, 0.6f);

    // Text
    ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 0.9f);

}

//void UI::CreateUsingFont()
//{
//    ImGuiIO& io = ImGui::GetIO();
//    io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\malgun.ttf)", 14.0f, NULL, io.Fonts->GetGlyphRangesKorean());
//
//    ImFontConfig FeatherFontConfig;
//    FeatherFontConfig.PixelSnapH = true;
//    FeatherFontConfig.FontDataOwnedByAtlas = false;
//    FeatherFontConfig.GlyphOffset = ImVec2(0, 0);
//    static constexpr ImWchar IconRanges[] = {
//        ICON_MOVE,      ICON_MOVE + 1,
//        ICON_ROTATE,    ICON_ROTATE + 1,
//        ICON_SCALE,     ICON_SCALE + 1,
//        ICON_MONITOR,   ICON_MONITOR + 1,
//        ICON_BAR_GRAPH, ICON_BAR_GRAPH + 1,
//        ICON_NEW,       ICON_NEW + 1,
//        ICON_SAVE,      ICON_SAVE + 1,
//        ICON_LOAD,      ICON_LOAD + 1,
//        0 };
//
//    io.Fonts->AddFontFromMemoryTTF(FeatherRawData, FontSizeOfFeather, 22.0f, &FeatherFontConfig, IconRanges);
//}