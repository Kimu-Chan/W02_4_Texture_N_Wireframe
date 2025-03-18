#include "pch.h"
#include "Box.h"

#include "CoreUObject/Components/SceneComponent.h"
#include "Debugging/DebugConsole.h"
#include "CoreUObject/Components/PrimitiveComponent.h"


FBox::FBox(USceneComponent* InOwner, const FVector& InMin, const FVector& InMax)
    : Owner(InOwner), Min(InMin), Max(InMax)
{
}

bool FBox::IsValidBox() const
{
    return Min.X < Max.X && Min.Y < Max.Y && Min.Z < Max.Z;
}

bool FBox::IntersectRay(const FRay& Ray, float& OutDistance) const
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

    // Ray가 AABB 뒤에서 시작함
    if (TMin < 0)
        return false;

    // 충돌하지 않음
    if (TMin > TMax)
        return false;

    OutDistance = TMin;
    return true;
}

void FBox::Update(const FMatrix& InModelMatrix)
{
	const FVector M0 = FVector(InModelMatrix.M[0][0], InModelMatrix.M[0][1], InModelMatrix.M[0][2]);
	const FVector M1 = FVector(InModelMatrix.M[1][0], InModelMatrix.M[1][1], InModelMatrix.M[1][2]);
	const FVector M2 = FVector(InModelMatrix.M[2][0], InModelMatrix.M[2][1], InModelMatrix.M[2][2]);
	const FVector M3 = FVector(InModelMatrix.M[3][0], InModelMatrix.M[3][1], InModelMatrix.M[3][2]);

	const FVector Origin = (InitialMin + InitialMax) * 0.5f;
	const FVector Extent = (InitialMax - InitialMin) * 0.5f;

    FVector NewOrigin = FVector::Replicate(Origin, 0) * M0;
    NewOrigin = FVector::Replicate(Origin, 1) * M1 + NewOrigin;
    NewOrigin = FVector::Replicate(Origin, 2) * M2 + NewOrigin;
    NewOrigin = M3 + NewOrigin;


	FVector NewExtent = FVector::Abs(FVector::Replicate(Extent, 0) * M0);
	NewExtent += FVector::Abs(FVector::Replicate(Extent, 1) * M1);
	NewExtent += FVector::Abs(FVector::Replicate(Extent, 2) * M2);


	Min = NewOrigin - NewExtent;
	Max = NewOrigin + NewExtent;
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