#include "pch.h" 
#include "Camera.h"
#include "CoreUObject/Components/PrimitiveComponent.h"


REGISTER_CLASS(ACamera);
ACamera::ACamera()
{
    bIsGizmo = true;
    
    NearClip = 0.1f;
    FarClip = 100.f;
    FieldOfView = 90.f;
    ProjectionMode = ECameraProjectionMode::Perspective;

    RootComponent = AddComponent<USceneComponent>();

    FTransform StartTransform = GetActorTransform();
    FVector StartLocation(3.f, -2.f, 2.f);
    StartTransform.Translate(StartLocation);
	StartTransform.LookAt(FVector::ZeroVector);
    SetActorTransform(StartTransform);
}

void ACamera::SetFieldOfView(float Fov)
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
    return FieldOfView;
}

float ACamera::GetNearClip() const
{
    return NearClip;
}

float ACamera::GetFarClip() const
{
    return FarClip;
}
