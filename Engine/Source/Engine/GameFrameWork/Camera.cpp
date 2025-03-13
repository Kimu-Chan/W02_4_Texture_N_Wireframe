#include "pch.h" 
#include "Camera.h"
#include "Core/Rendering/URenderer.h"
#include "CoreUObject/Components/PrimitiveComponent.h"


ACamera::ACamera()
    : NearClip(0.1f)
    , FarClip(100.f)
    , FieldOfView(90.f)
    , MaxPitch(89.8f)
    , CameraSpeed(1.f)
    , ProjectionMode(ECameraProjectionMode::Perspective)
{
    RootComponent = AddComponent<USceneComponent>();

    bIsGizmo = true;
    
    FTransform StartPos = GetActorTransform();
    StartPos.SetPosition(FVector(-5, 1, 2));
    SetActorTransform(StartPos);
}
