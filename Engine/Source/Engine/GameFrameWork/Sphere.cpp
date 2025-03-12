#include "pch.h" 
#include "Sphere.h"
#include "CoreUObject/Components/PrimitiveComponent.h"

ASphere::ASphere()
{
    bCanEverTick = true;

    USphereComp* SphereComponent = AddComponent<USphereComp>();
    RootComponent = SphereComponent;
        
    SetActorTransform(FTransform());
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
