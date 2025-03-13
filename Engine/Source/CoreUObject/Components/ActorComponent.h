#pragma once

#include "Engine/EngineTypes.h"
#include "CoreUObject/Object.h"

class UActorComponent : public UObject
{
public:
	UActorComponent() = default;

	virtual void BeginPlay();
	virtual void TickComponent(float DeltaTime);
	virtual void EndPlay(const EEndPlayReason::Type Reason);

	bool CanEverTick() const { return bCanEverTick; }

	virtual class AActor* GetOwner() const;
	virtual void SetOwner(class AActor* InOwner) { Owner = InOwner; }

protected:
	bool bCanEverTick = true;
	class AActor* Owner = nullptr;
};

