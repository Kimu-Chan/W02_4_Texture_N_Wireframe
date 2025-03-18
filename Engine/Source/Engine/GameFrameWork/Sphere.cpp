#include "pch.h" 
#include "Sphere.h"
#include "CoreUObject/Components/PrimitiveComponent.h"
#include "../../../Billboard.h"
#include "Engine/Engine.h"


REGISTER_CLASS(ASphere);
ASphere::ASphere()
{
    bCanEverTick = true;

    USphereComp* SphereComponent = AddComponent<USphereComp>();
    RootComponent = SphereComponent;
}

void ASphere::BeginPlay()
{
    Super::BeginPlay();
}

void ASphere::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

const char* ASphere::GetTypeName()
{
    return "Sphere";
}
