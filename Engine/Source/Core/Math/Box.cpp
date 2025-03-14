#include "pch.h"
#include "Box.h"

#include "CoreUObject/Components/SceneComponent.h"
#include "Debugging/DebugConsole.h"


FBox::FBox(USceneComponent* InOwner, const FVector& InMin, const FVector& InMax)
    : Owner(InOwner), Min(InMin), Max(InMax)
{
}

bool FBox::IsValidBox() const
{
    return Min.X < Max.X && Min.Y < Max.Y && Min.Z < Max.Z;
}

bool FBox::IntersectRay(const FRay& Ray) const
{
    USceneComponent* debug = Owner;

    FVector NormalizedDir = Ray.Direction.GetSafeNormal();
    // 1. Ray의 방향 역수
    FVector InvDir{
        1.0f / NormalizedDir.X,
        1.0f / NormalizedDir.Y,
        1.0f / NormalizedDir.Z
    };

    // 2. Ray의 방향에 따라 TMin, TMax 계산
    float T1 = (Min.X - Ray.Origin.X) * InvDir.X;
    float T2 = (Max.X - Ray.Origin.X) * InvDir.X;
    float T3 = (Min.Y - Ray.Origin.Y) * InvDir.Y;
    float T4 = (Max.Y - Ray.Origin.Y) * InvDir.Y;
    float T5 = (Min.Z - Ray.Origin.Z) * InvDir.Z;
    float T6 = (Max.Z - Ray.Origin.Z) * InvDir.Z;

    float TMin = FMath::Max(FMath::Max(FMath::Min(T1, T2), FMath::Min(T3, T4)), FMath::Min(T5, T6));
    float TMax = FMath::Min(FMath::Min(FMath::Max(T1, T2), FMath::Max(T3, T4)), FMath::Max(T5, T6));

    UE_LOG("Ray Origin: %.2f, %.2f, %.2f", Ray.Origin.X, Ray.Origin.Y, Ray.Origin.Z);
    UE_LOG("Ray Direction: %.2f, %.2f, %.2f", NormalizedDir.X, NormalizedDir.Y, NormalizedDir.Z);

    // Ray가 AABB 뒤에서 시작함
    if (TMin < 0)
        return false;

    // 충돌하지 않음
    if (TMin > TMax)
        return false;

    return true;
}

void FBox::Update(const FMatrix& InModelMatrix)
{
    FVector Vertices[8] =
    {
        InModelMatrix.TransformPosition(FVector(InitialMin.X, InitialMin.Y, InitialMin.Z)),
        InModelMatrix.TransformPosition(FVector(InitialMin.X, InitialMin.Y, InitialMax.Z)),
        InModelMatrix.TransformPosition(FVector(InitialMin.X, InitialMax.Y, InitialMin.Z)),
        InModelMatrix.TransformPosition(FVector(InitialMin.X, InitialMax.Y, InitialMax.Z)),
        InModelMatrix.TransformPosition(FVector(InitialMax.X, InitialMin.Y, InitialMin.Z)),
        InModelMatrix.TransformPosition(FVector(InitialMax.X, InitialMin.Y, InitialMax.Z)),
        InModelMatrix.TransformPosition(FVector(InitialMax.X, InitialMax.Y, InitialMin.Z)),
        InModelMatrix.TransformPosition(FVector(InitialMax.X, InitialMax.Y, InitialMax.Z))
    };

    FVector NewMin = Vertices[0];
    FVector NewMax = Vertices[0];

    for (int i = 1; i < 8; i++)
    {
        NewMin.X = FMath::Min(NewMin.X, Vertices[i].X);
        NewMin.Y = FMath::Min(NewMin.Y, Vertices[i].Y);
        NewMin.Z = FMath::Min(NewMin.Z, Vertices[i].Z);
        NewMax.X = FMath::Max(NewMax.X, Vertices[i].X);
        NewMax.Y = FMath::Max(NewMax.Y, Vertices[i].Y);
        NewMax.Z = FMath::Max(NewMax.Z, Vertices[i].Z);
    }

    Min = NewMin;
    Max = NewMax;
}

void FBox::Init(USceneComponent* InOwner, const FVector& InMin, const FVector& InMax)
{
    Owner = InOwner;
    FTransform WorldTransform = InOwner->GetWorldTransform();
    Update(WorldTransform.GetMatrix());

    InitialMin = Min = InMin;
    InitialMax = Max = InMax;
}

void FBox::Init(USceneComponent* InOwner, const FVector& InCenter, float InRadius)
{
    Owner = InOwner;
    Min = InCenter - FVector(InRadius, InRadius, InRadius);
    Max = InCenter + FVector(InRadius, InRadius, InRadius);
}

FVector FBox::GetCenter() const
{
    return (Min + Max) * 0.5f;
}

USceneComponent* FBox::GetOwner() const
{
    return Owner;
}