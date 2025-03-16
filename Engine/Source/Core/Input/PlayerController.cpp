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

void APlayerController::HandleCameraMovement(float DeltaTime) {

	//@TODO: ImGuiIO.WantCaptureMouse를 이용하여 UI 조작중에는 카메라 조작을 막아야함
    FVector NewVelocity(0, 0, 0);

    ACamera* Camera = FEditorManager::Get().GetCamera();
    FTransform CameraTransform = Camera->GetActorTransform();

    if (APlayerInput::Get().IsPressedMouse(true))
    {
        // Last Frame Mouse Position
        FVector MousePrePos = APlayerInput::Get().GetMousePrePos();
        FVector MousePos = APlayerInput::Get().GetMousePos();
        FVector DeltaPos = MousePos - MousePrePos;

        FVector NewRotation = CameraTransform.GetRotation().GetEuler();
        NewRotation.Y += MouseSensitivity * DeltaPos.Y * DeltaTime; // Pitch
        NewRotation.Z += MouseSensitivity * DeltaPos.X * DeltaTime; // Yaw

        NewRotation.Y = FMath::Clamp(NewRotation.Y, -Camera->MaxYDegree, Camera->MaxYDegree);
        CameraTransform.SetRotation(NewRotation);
    }

    // Camera Speed //
    float MouseWheel = APlayerInput::Get().GetMouseWheel();
    CurrentSpeed += MouseWheel * 0.1f;
    CurrentSpeed = FMath::Clamp(CurrentSpeed, MinSpeed, MaxSpeed);

    APlayerInput::Get().SetMouseWheel(0.0f);    // Reset MouseWheel After Set Camera Speed

    if (APlayerInput::Get().IsPressedKey(EKeyCode::A))
    {
        NewVelocity -= Camera->GetRight();
    }
    if (APlayerInput::Get().IsPressedKey(EKeyCode::D))
    {
        NewVelocity += Camera->GetRight();
    }
    if (APlayerInput::Get().IsPressedKey(EKeyCode::W))
    {
        NewVelocity += Camera->GetForward();
    }
    if (APlayerInput::Get().IsPressedKey(EKeyCode::S))
        {
        NewVelocity -= Camera->GetForward();
    }
    if (APlayerInput::Get().IsPressedKey(EKeyCode::Q))
    {
        NewVelocity -= {0.0f, 0.0f, 1.0f};
    }
    if (APlayerInput::Get().IsPressedKey(EKeyCode::E))
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
}

void APlayerController::ProcessPlayerInput(float DeltaTime) {

    HandleGizmoMovement(DeltaTime);
    HandleCameraMovement(DeltaTime);
}