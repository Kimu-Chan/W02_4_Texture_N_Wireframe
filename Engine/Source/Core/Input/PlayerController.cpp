#include "pch.h" 
#include "PlayerController.h"

#include "Static/EditorManager.h"
#include "PlayerInput.h"
#include "Core/Math/Plane.h"
#include "Engine/GameFrameWork/Camera.h"
#include "Engine/EngineConfig.h"

APlayerController::APlayerController() {

}

void APlayerController::HandleCameraMovement(float DeltaTime) {

	//@TODO: ImGuiIO.WantCaptureMouse를 이용하여 UI 조작중에는 카메라 조작을 막아야함
    FVector NewVelocity(0, 0, 0);

    if (APlayerInput::Get().IsPressedMouse(true) == false)
    {
        // Camera->SetVelocity(NewVelocity);
        return;
    }

    ACamera* Camera = FEditorManager::Get().GetCamera();

    // Camera Speed //
    float MouseWheel = APlayerInput::Get().GetMouseWheel();

    float CameraSpeed = Camera->CameraSpeed;
    CameraSpeed += MouseWheel * 0.1f;
    CameraSpeed = FMath::Clamp(CameraSpeed, 0.1f, 5.0f);

    Camera->CameraSpeed = CameraSpeed;

    APlayerInput::Get().SetMouseWheel(0.0f);    // Reset MouseWheel After Set Camera Speed

    // Last Frame Mouse Position
    FVector MousePrePos = APlayerInput::Get().GetMousePrePos();
    FVector MousePos = APlayerInput::Get().GetMousePos();
    FVector DeltaPos = MousePos - MousePrePos;
    //FQuat CameraRot = FEditorManager::Get().GetCamera()->GetActorTransform().GetRotation();

    FTransform CameraTransform = Camera->GetActorTransform();

    FVector TargetRotation = CameraTransform.GetRotation().GetEuler();
    TargetRotation.Y += Camera->CameraSpeed * DeltaPos.Y * DeltaTime * 2.0f;    //@TODO: CameraRotaionSpeed를 나중에 조절할 수 있도록 수정
    TargetRotation.Z += Camera->CameraSpeed * DeltaPos.X * DeltaTime * 2.0f;

    TargetRotation.Y = FMath::Clamp(TargetRotation.Y, -Camera->MaxYDegree, Camera->MaxYDegree);
    CameraTransform.SetRotation(TargetRotation);

    if (APlayerInput::Get().IsPressedKey(EKeyCode::A)) {
        NewVelocity -= Camera->GetRight();
    }
    if (APlayerInput::Get().IsPressedKey(EKeyCode::D)) {
        NewVelocity += Camera->GetRight();
    }
    if (APlayerInput::Get().IsPressedKey(EKeyCode::W)) {
        NewVelocity += Camera->GetForward();
    }
    if (APlayerInput::Get().IsPressedKey(EKeyCode::S)) {
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

    //회전이랑 마우스클릭 구현 카메라로 해야할듯?
    CameraTransform.Translate(NewVelocity * DeltaTime * Camera->CameraSpeed);
    Camera->SetActorTransform(CameraTransform);
    // FCamera::Get().SetVelocity(NewVelocity);
    SaveCameraProperties(Camera);
	

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

void APlayerController::SaveCameraProperties(ACamera* Camera)
{
	FEngineConfig* EngineConfig = UEngine::Get().GetEngineConfig();
	FTransform CameraTransform = Camera->GetActorTransform();

	float FOV = Camera->GetFieldOfView();
	float NearClip = Camera->GetNearClip();
	float FarClip = Camera->GetFarClip();
	float CameraSpeed = Camera->CameraSpeed;

    UEngine::Get().GetEngineConfig()->SaveEngineConfig(EEngineConfigValueType::EEC_EditorCameraPosX, CameraTransform.GetPosition().X);
    UEngine::Get().GetEngineConfig()->SaveEngineConfig(EEngineConfigValueType::EEC_EditorCameraPosY, CameraTransform.GetPosition().Y);
    UEngine::Get().GetEngineConfig()->SaveEngineConfig(EEngineConfigValueType::EEC_EditorCameraPosZ, CameraTransform.GetPosition().Z);

	UEngine::Get().GetEngineConfig()->SaveEngineConfig(EEngineConfigValueType::EEC_EditorCameraRotX, CameraTransform.GetRotation().X);
	UEngine::Get().GetEngineConfig()->SaveEngineConfig(EEngineConfigValueType::EEC_EditorCameraRotY, CameraTransform.GetRotation().Y);
	UEngine::Get().GetEngineConfig()->SaveEngineConfig(EEngineConfigValueType::EEC_EditorCameraRotZ, CameraTransform.GetRotation().Z);
	UEngine::Get().GetEngineConfig()->SaveEngineConfig(EEngineConfigValueType::EEC_EditorCameraRotW, CameraTransform.GetRotation().W);

	UEngine::Get().GetEngineConfig()->SaveEngineConfig(EEngineConfigValueType::EEC_EditorCameraSpeed, CameraSpeed);

}

void APlayerController::ProcessPlayerInput(float DeltaTime) {

    HandleGizmoMovement(DeltaTime);
    HandleCameraMovement(DeltaTime);
}