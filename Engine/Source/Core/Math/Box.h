#pragma once


#include "Vector.h"
#include "Transform.h"
#include "Ray.h"

class USceneComponent;


struct FBox
{
public:
    FBox() = default;
    FBox(USceneComponent* InOwner, const FVector& InMin, const FVector& InMax);

    bool IsValidBox() const;
    bool IntersectRay(const FRay& Ray) const;
    void Update(const FMatrix& InModelMatrix);
    void Init(USceneComponent* InOwner, const FVector& InMin, const FVector& InMax);
    void Init(USceneComponent* InOwner, const FVector& InCenter, float InRadius);
    FVector GetCenter() const;
    USceneComponent* GetOwner() const;

    FVector Min;
    FVector Max;
    FVector InitialMin;
    FVector InitialMax;
    USceneComponent* Owner = nullptr;
    bool bCanBeRendered = false;
};