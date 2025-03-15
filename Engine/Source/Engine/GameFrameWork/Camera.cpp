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

    FVector StartLocation(3.f, 2.f, 2.f);
    
    FVector Delta = (FVector::ZeroVector - StartLocation).GetSafeNormal();
    float Pitch = FMath::RadiansToDegrees(asinf(Delta.Z)) * -1;
    float Yaw = FMath::RadiansToDegrees(atan2(Delta.Y, Delta.X));
    FVector StartRotation(0.f, Pitch, Yaw);
    
    FTransform StartTransform = GetActorTransform();
    StartTransform.SetPosition(StartLocation);
    StartTransform.SetRotation(StartRotation);
    
    SetActorTransform(StartTransform);
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
