#pragma once

#include "Core/Container/Set.h"
#include "CoreUObject/Components/ActorComponent.h"
#include "Core/Math/Transform.h"

struct FVector;
struct FMatrix;
struct FTransform;
struct FBox;

class USceneComponent : public UActorComponent
{
	UCLASS(USceneComponent, UActorComponent);
	friend class AActor;
	using Super = UActorComponent;
public:
	USceneComponent() = default;

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;


	/* 로컬 트랜스폼을 반환*/
	FTransform GetComponentTransform() const { return RelativeTransform; }
	/* 월드 트랜스폼을 반환, 이걸로 렌더링한다*/
	const FTransform GetWorldTransform();

	void SetRelativeTransform(const FTransform& InTransform);

	void Pick(bool bPicked);
public:
	bool IsPicked() const { return bIsPicked; }

public:
	void SetupAttachment(USceneComponent* InParent, bool bUpdateChildTransform = false);
	// 부모의 월드 트랜스폼을 받아서 자신의 로컬 트랜스폼을 갱신
	void ApplyParentWorldTransform(const FTransform& InTransform);

protected:
	USceneComponent* Parent = nullptr;
	TSet<USceneComponent*> Children;
	// 이건 내 로컬 트랜스폼
	FTransform RelativeTransform = FTransform();
	bool bCanEverTick = true;


	// 바운딩 박스
protected:
	// !TODO : 런타임에 박스를 켜고 끄는 함수
	std::shared_ptr<FBox> BoundingBox = nullptr;
	virtual void InitBoundingBox() {};
	virtual void UpdateBoundingBox() {};

protected:
	bool bIsPicked = false;
};