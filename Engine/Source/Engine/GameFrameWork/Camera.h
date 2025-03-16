#pragma once

#include "Core/Math/Vector.h"
#include "Core/Math/Matrix.h"
#include "Core/Math/Transform.h"
#include "Core/HAL/PlatformType.h"
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
    UCLASS(ACamera, AActor);
    using Super = AActor;
    
public:
    ACamera();

private:    
    float NearClip;
    float FarClip;
    // 화면각
    float FieldOfView;

public:
    const float MaxYDegree = 89.8f;
    
    // 투영 타입 - Perspective, Orthographic
    ECameraProjectionMode::Type ProjectionMode;
    // float AspectRatio;	// 카메라 비율 (이번 프로젝트에서는 사용 안할듯) 

    void SetFieldOfVew(float Fov);
    void SetFar(float Far);
    void SetNear(float Near);
    
    float GetFieldOfView() const;
    float GetNearClip() const;
    float GetFarClip() const;

        
    FVector GetForward() const
    {
        return GetActorTransform().GetForward();
    }
    
    FVector GetRight() const
    {
        return GetActorTransform().GetRight();
    }
    
    FVector GetUp() const
    {
        return GetActorTransform().GetUp();
    }

    FMatrix GetViewMatrix() const
    {
        return GetActorTransform().GetViewMatrix();
    }
};