#pragma once

#include "Core/Math/Vector.h"
#include "Core/Math/Matrix.h"
#include "Core/Math/Transform.h"
#include "Actor.h"

namespace ECameraProjectionMode
{
    enum Type : uint8
    {
        Perspective,
        Orthographic
    };
}

class ACamera : public AActor
{

    using Super = AActor;
    
public:
    ACamera();

private:    
    float NearClip;
    float FarClip;
    // 화면각
    float FieldOfView;

public:
    const float MaxPitch;
    //카메라 스피드 IMGui용 나중에 Velocity로 관리하면 없어질애라 편하게 public에서 관리
    float CameraSpeed;
    
    // 투영 타입 - Perspective, Orthographic
    ECameraProjectionMode::Type ProjectionMode;
    // float AspectRatio;	// 카메라 비율 (이번 프로젝트에서는 사용 안할듯) 

    void SetFieldOfVew(float InFieldOfView) { FieldOfView = InFieldOfView; }
    void SetFar(float InFarClip) { FarClip = InFarClip; }
    void SetNear(float InNearClip) { NearClip = InNearClip; }
    
    float GetFieldOfView() const { return FieldOfView; }
    float GetNearClip() const { return NearClip; }
    float GetFarClip() const { return FarClip; }
    
    FVector GetForwardVector() const { return GetActorTransform().GetForward(); }
    FVector GetRightVector() const { return GetActorTransform().GetRight(); }
    FVector GetUpVector() const { return GetActorTransform().GetUp(); }
    FMatrix GetViewMatrix() const { return GetActorTransform().GetViewMatrix(); }
};