#include "pch.h" 
#include "Cone.h"
#include "CoreUObject/Components/PrimitiveComponent.h"

REGISTER_CLASS(ACone);
ACone::ACone()
{
    bCanEverTick = true;

    UConeComp* ConeComponent = AddComponent<UConeComp>();
    RootComponent = ConeComponent;
        
    SetActorTransform(FTransform());
}

void ACone::BeginPlay()
{
    Super::BeginPlay();
}

void ACone::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

const char* ACone::GetTypeName()
{
    return "Cone";
}
