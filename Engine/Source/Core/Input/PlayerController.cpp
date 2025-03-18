#include "pch.h" 
#include "PlayerController.h"

#include "Static/EditorManager.h"
#include "PlayerInput.h"
#include "Core/Math/Plane.h"
#include "Engine/Engine.h"
#include "Engine/GameFrameWork/Camera.h"
#include "Engine/EngineConfig.h"
#include "Rendering/URenderer.h"

APlayerController::APlayerController()
    : CurrentSpeed(3.f)
    , MaxSpeed(10.f)
    , MinSpeed(1.0f)
    , MouseSensitivity(10.f)
    , MaxSensitivity(20.f)
    , MinSensitivity(1.f)
{}

APlayerController::~APlayerController()
{
}

void APlayerController::HandleCameraMovement(float DeltaTime)
{
    if (bUiCaptured)
    {
        return;
    }
    
    if (!APlayerInput::Get().IsMouseDown(true))
    {
        if (APlayerInput::Get().IsMouseReleased(true))
        {
            /**
             * ShowCursor 함수는 참조 카운트를 하므로, 정확한 횟수만큼 Show 및 Hide 하지 않으면
             * 의도대로 작동하지 않는 문제가 발생함으로 매우 주의해야 함.
             */
            ShowCursor(true);
        }
        return;
    }
    
    FVector NewVelocity(0, 0, 0);

    ACamera* Camera = FEditorManager::Get().GetCamera();
    FTransform CameraTransform = Camera->GetActorTransform();

    // Look
    int32 DeltaX = 0;
    int32 DeltaY = 0;
    APlayerInput::Get().GetMouseDelta(DeltaX, DeltaY);
        
    FVector NewRotation = CameraTransform.GetRotation().GetEuler();
    NewRotation.Y += MouseSensitivity * static_cast<float>(DeltaY) * DeltaTime; // Pitch
    NewRotation.Z += MouseSensitivity * static_cast<float>(DeltaX) * DeltaTime; // Yaw

    NewRotation.Y = FMath::Clamp(NewRotation.Y, -Camera->MaxYDegree, Camera->MaxYDegree);
    CameraTransform.SetRotation(NewRotation);

    if (APlayerInput::Get().IsMousePressed(true))
    {
        // Press 이벤트 발생시 커서 위치를 캐싱하여 해당 위치로 커서를 고정시킴.
        APlayerInput::Get().CacheCursorPosition();
        ShowCursor(false);
    }
    APlayerInput::Get().FixMouseCursor();
    
    // Move
    int32 MouseWheel = APlayerInput::Get().GetMouseWheelDelta();
    CurrentSpeed += static_cast<float>(MouseWheel) * 0.005f;
    CurrentSpeed = FMath::Clamp(CurrentSpeed, MinSpeed, MaxSpeed);

    if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::A))
    {
        NewVelocity -= Camera->GetRight();
    }
    if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::D))
    {
        NewVelocity += Camera->GetRight();
    }
    if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::W))
    {
        NewVelocity += Camera->GetForward();
    }
    if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::S))
        {
        NewVelocity -= Camera->GetForward();
    }
    if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::Q))
    {
        NewVelocity -= {0.0f, 0.0f, 1.0f};
    }
    if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::E))
    {
        NewVelocity += {0.0f, 0.0f, 1.0f};
    }
    if (NewVelocity.Length() > 0.001f)
    {
        NewVelocity = NewVelocity.GetSafeNormal();
    }

    CameraTransform.Translate(NewVelocity * DeltaTime * CurrentSpeed);
    Camera->SetActorTransform(CameraTransform);
    
    SaveCameraProperties(Camera);
	
	UEngine::Get().GetRenderer()->UpdateViewMatrix(CameraTransform);
}

void APlayerController::SaveCameraProperties(ACamera* Camera)
{
    FEngineConfig* EngineConfig = UEngine::Get().GetEngineConfig();
    FTransform CameraTransform = Camera->GetActorTransform();

    float FOV = Camera->GetFieldOfView();
    float NearClip = Camera->GetNearClip();
    float FarClip = Camera->GetFarClip();
    float CameraSpeed = CurrentSpeed;

    UEngine::Get().GetEngineConfig()->SaveEngineConfig(EEngineConfigValueType::EEC_EditorCameraPosX, CameraTransform.GetPosition().X);
    UEngine::Get().GetEngineConfig()->SaveEngineConfig(EEngineConfigValueType::EEC_EditorCameraPosY, CameraTransform.GetPosition().Y);
    UEngine::Get().GetEngineConfig()->SaveEngineConfig(EEngineConfigValueType::EEC_EditorCameraPosZ, CameraTransform.GetPosition().Z);

    UEngine::Get().GetEngineConfig()->SaveEngineConfig(EEngineConfigValueType::EEC_EditorCameraRotX, CameraTransform.GetRotation().X);
    UEngine::Get().GetEngineConfig()->SaveEngineConfig(EEngineConfigValueType::EEC_EditorCameraRotY, CameraTransform.GetRotation().Y);
    UEngine::Get().GetEngineConfig()->SaveEngineConfig(EEngineConfigValueType::EEC_EditorCameraRotZ, CameraTransform.GetRotation().Z);
    UEngine::Get().GetEngineConfig()->SaveEngineConfig(EEngineConfigValueType::EEC_EditorCameraRotW, CameraTransform.GetRotation().W);
}

void APlayerController::SetCurrentSpeed(float InSpeed)
{
    CurrentSpeed = FMath::Clamp(InSpeed, MinSpeed, MaxSpeed);
    UEngine::Get().GetEngineConfig()->SaveEngineConfig(EEngineConfigValueType::EEC_EditorCameraSpeed, CurrentSpeed);
}

void APlayerController::SetMouseSensitivity(float InSensitivity)
{
    MouseSensitivity = FMath::Clamp(InSensitivity, MinSensitivity, MaxSensitivity);
    UEngine::Get().GetEngineConfig()->SaveEngineConfig(EEngineConfigValueType::EEC_EditorCameraSensitivity, MouseSensitivity);
}

void APlayerController::ProcessPlayerInput(float DeltaTime)
{
    if (bUiInput)
    {
        if (APlayerInput::Get().IsMouseDown(true) || APlayerInput::Get().IsMouseDown(false))
        {
            bUiCaptured = true;
        }
        return;
    }
    if (bUiCaptured)
    {
        if (!APlayerInput::Get().IsMouseDown(true) && !APlayerInput::Get().IsMouseDown(false))
        {
            bUiCaptured = false;
        }
        return;
    }
    
    // TODO: 기즈모 조작시에는 카메라 입력 무시
    // HandleGizmoMovement(DeltaTime); // TODO: 의미없는 함수인듯
    HandleCameraMovement(DeltaTime);
}
