#include "pch.h" 
#include "Camera.h"
#include "Core/Rendering/URenderer.h"
#include "CoreUObject/Components/PrimitiveComponent.h"


REGISTER_CLASS(ACamera);
ACamera::ACamera()
{
    bIsGizmo = true;
    
    NearClip = 0.1f;
    FarClip = 100.f;
    FieldOfView = 45.f;
    ProjectionMode = ECameraProjectionMode::Perspective;

    RootComponent = AddComponent<USceneComponent>();
    
    FTransform StartPos = GetActorTransform();
    StartPos.SetPosition(FVector(-5, 0, 0));
    SetActorTransform(StartPos);
}

void ACamera::SetFieldOfVew(float Fov)
{
    FieldOfView = Fov;
}

void ACamera::SetFar(float Far)
{
    this->FarClip = Far;
}

void ACamera::SetNear(float Near)
{
    this->NearClip = Near;
}

float ACamera::GetFieldOfView() const
{
    return  FieldOfView;
}

float ACamera::GetNearClip() const
{
    return NearClip;
}

float ACamera::GetFarClip() const
{
    return FarClip;
}
