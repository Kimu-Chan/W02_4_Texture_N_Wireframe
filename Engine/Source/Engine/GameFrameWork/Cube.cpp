#include "pch.h" 
#include "Cube.h"
#include "CoreUObject/Components/PrimitiveComponent.h"

ACube::ACube()
{
    bCanEverTick = true;

    UCubeComp* CubeComponent = AddComponent<UCubeComp>();
    RootComponent = CubeComponent;

    CubeComponent->SetRelativeTransform(FTransform());
}

void ACube::BeginPlay()
{
    Super::BeginPlay();
}

void ACube::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

const char* ACube::GetTypeName()
{
    return "Cube";
}
