#include "pch.h" 
#include "Arrow.h"
#include "CoreUObject/Components/PrimitiveComponent.h"

AArrow::AArrow()
{
    bCanEverTick = true;

    UCylinderComp* CylinderComp = AddComponent<UCylinderComp>();
    RootComponent = CylinderComp;

    RootComponent->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.5f), FQuat(0, 0, 0, 1), FVector(1.f, 1.f, 1.f)));

    UConeComp* ConeComp = AddComponent<UConeComp>();
    ConeComp->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 1.0f), FQuat(0, 0, 0, 1), FVector(1.2f, 1.2f, 0.5f)));

    ConeComp->SetupAttachment(RootComponent);
}

void AArrow::BeginPlay()
{
    Super::BeginPlay();
}

void AArrow::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

const char* AArrow::GetTypeName()
{
    return "Arrow";
}
