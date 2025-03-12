#include "pch.h" 
#include "Cylinder.h"
#include "CoreUObject/Components/PrimitiveComponent.h"

ACylinder::ACylinder()
{
    bCanEverTick = true;

    UCylinderComp* CylinderComponent = AddComponent<UCylinderComp>();
    RootComponent = CylinderComponent;
        
    SetActorTransform(FTransform());
}

void ACylinder::BeginPlay()
{
    Super::BeginPlay();
}

void ACylinder::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

const char* ACylinder::GetTypeName()
{
    return "Cylinder";
}
