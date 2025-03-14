#include "pch.h"
#include "ActorComponent.h"
#include "Engine/GameFrameWork/Actor.h"

REGISTER_CLASS(UActorComponent);
void UActorComponent::BeginPlay()
{
}

void UActorComponent::Tick(float DeltaTime)
{
}

void UActorComponent::EndPlay(const EEndPlayReason::Type Reason)
{
}

AActor* UActorComponent::GetOwner() const
{
    return Owner;
}
