#include "pch.h" 
#include "PlayerController.h"

#include "Static/EditorManager.h"
#include "PlayerInput.h"
#include "Core/Math/Plane.h"
#include "Engine/GameFrameWork/Camera.h"

APlayerController::APlayerController()
    : CurrentSpeed(3.f)
    , MaxSpeed(10.f)
    , MinSpeed(0.1f)
    , MouseSensitivity(10.f)
{}

void APlayerController::HandleCameraMovement(float DeltaTime)
{
    if (!APlayerInput::Get().GetMouseDown(true))
    {
        return;
    }
    
	//@TODO: ImGuiIO.WantCaptureMouse를 이용하여 UI 조작중에는 카메라 조작을 막아야함
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
    
    // Move
    int32 MouseWheel = APlayerInput::Get().GetMouseWheelDelta();
    CurrentSpeed += static_cast<float>(MouseWheel) * 0.001f;
    CurrentSpeed = FMath::Clamp(CurrentSpeed, MinSpeed, MaxSpeed);

    if (APlayerInput::Get().GetKeyDown(DirectX::Keyboard::Keys::A))
    {
        NewVelocity -= Camera->GetRight();
    }
    if (APlayerInput::Get().GetKeyDown(DirectX::Keyboard::Keys::D))
    {
        NewVelocity += Camera->GetRight();
    }
    if (APlayerInput::Get().GetKeyDown(DirectX::Keyboard::Keys::W))
    {
        NewVelocity += Camera->GetForward();
    }
    if (APlayerInput::Get().GetKeyDown(DirectX::Keyboard::Keys::S))
        {
        NewVelocity -= Camera->GetForward();
    }
    if (APlayerInput::Get().GetKeyDown(DirectX::Keyboard::Keys::Q))
    {
        NewVelocity -= {0.0f, 0.0f, 1.0f};
    }
    if (APlayerInput::Get().GetKeyDown(DirectX::Keyboard::Keys::E))
    {
        NewVelocity += {0.0f, 0.0f, 1.0f};
    }
    if (NewVelocity.Length() > 0.001f)
    {
        NewVelocity = NewVelocity.GetSafeNormal();
    }

    CameraTransform.Translate(NewVelocity * DeltaTime * CurrentSpeed);
    Camera->SetActorTransform(CameraTransform);
}

void APlayerController::HandleGizmoMovement(float DeltaTime)
{
    bIsHandlingGizmo = false;
    
    if (APlayerInput::Get().IsPressedMouse(false) == false)
    {
        return;
    }

    AActor* SelectedActor = FEditorManager::Get().GetSelectedActor();

    //@TODO: Check with Gizmo
    if (SelectedActor == nullptr)
    {
        return;
    }

    bIsHandlingGizmo = true;
}

void APlayerController::ProcessPlayerInput(float DeltaTime)
{
    // TODO: 기즈모 조작시에는 카메라 입력 무시
    // HandleGizmoMovement(DeltaTime);
    HandleCameraMovement(DeltaTime);
}