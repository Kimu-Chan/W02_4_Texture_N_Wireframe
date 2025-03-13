#pragma once

#include "Core/Math/Vector.h"
#include "Core/Math/Matrix.h"
#include "Core/Math/Transform.h"
#include "Core/Container/Set.h"
#include "CoreUObject/Object.h"
#include "CoreUObject/Components/ActorComponent.h"

class USceneComponent : public UActorComponent
{
	friend class AActor;
	using Super = UActorComponent;
public:
	USceneComponent() = default;

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime) override;

	/* 로컬 트랜스폼을 반환*/
	FTransform GetRelativeTransform() const { return {RelativeLocation, RelativeRotation, RelativeScale}; }

	/* 월드 트랜스폼을 반환, 이걸로 렌더링한다*/
	const FTransform GetWorldTransform();

	void SetRelativeTransform(const FTransform& InTransform);

	FVector GetRelativeLocation() const { return RelativeLocation; }
	FVector GetRelativeRotation() const { return RelativeRotation; }
	FVector GetRelativeScale() const { return RelativeScale; }

	void SetRelativeLocation(const FVector& InLocation) { RelativeLocation = InLocation; }
	void SetRelativeRotation(const FVector& InRotation) { RelativeRotation = InRotation; }
	void SetRelativeScale(const FVector& InScale) { RelativeScale = InScale; }

	void Pick(bool bPicked);

	bool IsPicked() const { return bIsPicked; }

	void SetupAttachment(USceneComponent* InParent, bool bUpdateChildTransform = false);

	// 부모의 월드 트랜스폼을 받아서 자신의 로컬 트랜스폼을 갱신
	void ApplyParentWorldTransform(const FTransform& InTransform);

protected:
	USceneComponent* AttachParent = nullptr;
	
	TSet<USceneComponent*> Children;

	FVector RelativeLocation = FVector::ZeroVector;

	FVector RelativeRotation = FVector::ZeroVector;

	FVector RelativeScale = FVector::OneVector;
	
	bool bCanEverTick = true;

	// debug
	bool bIsPicked = false;

private:
	AActor* OwnerPrivate = nullptr;
};