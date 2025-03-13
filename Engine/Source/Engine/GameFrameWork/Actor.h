#pragma once

#include "Core/Container/Set.h"
#include "Core/Math/Transform.h"
#include "CoreUObject/Object.h"
#include "CoreUObject/ObjectFactory.h"
#include "CoreUObject/Components/ActorComponent.h"
#include "CoreUObject/Components/SceneComponent.h"
#include "Engine/EngineTypes.h"

class UWorld;

class AActor : public UObject
{
	friend class FEditorManager;
public:
	AActor();
	virtual ~AActor() override = default;

	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);
	virtual void LateTick (float DeltaTime); // 렌더 후 호출
	
	virtual void Destroyed();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	UWorld* GetWorld();
	
	TSet<UActorComponent*>& GetComponents() { return Components; }

	bool IsGizmoActor() const { return bIsGizmo; }

	void SetDepth(uint32 InDepth) { Depth = InDepth; }

	uint32 GetDepth() const { return Depth; }

	virtual FVector GetActorLocation() const;
	virtual FVector GetActorRotation() const;
	virtual FVector GetActorScale3D() const;

	virtual bool SetActorLocation(const FVector& InLocation);
	virtual bool SetActorRotation(const FVector& InRotation);
	virtual bool SetActorScale3D(const FVector& InScale);
	
private:
	virtual void OnActorPicked();
	virtual void OnActorUnPicked();

	uint32 Depth = 0;
	bool bIsPicked = false;

public:
	bool IsPicked() const { return bIsPicked; }

	template<typename T>
		requires std::derived_from<T, UActorComponent>
	T* AddComponent()
	{
		T* ObjectInstance = FObjectFactory::ConstructObject<T>();
		Components.Add(ObjectInstance);
		ObjectInstance->SetOwner(this);

		// 만약 SceneComponent를 상속 받았다면
		//if constexpr (std::is_base_of_v<USceneComponent, T>)
		//{
		//	if (RootComponent == nullptr)
		//	{
		//		RootComponent = ObjectInstance;
		//	}
		//	else
		//	{
		//		ObjectInstance->SetupAttachment(RootComponent);
		//	}
		//}

		return ObjectInstance;
	}

	// delete
	template<typename T>
		requires std::derived_from<T, UActorComponent>
	void RemoveComponent(T* Object)
	{
		Components.Remove(Object);
	}

	FTransform GetActorTransform() const;
	void SetActorTransform(const FTransform& InTransform);
	bool CanEverTick() const { return bCanEverTick; }
	virtual const char* GetTypeName();

	bool Destroy();

	USceneComponent* GetRootComponent() const { return RootComponent; }
	void SetRootComponent(USceneComponent* InRootComponent) { RootComponent = InRootComponent; }

	void SetColor(FVector4 InColor);
	void SetUseVertexColor(bool bUseVertexColor);

protected:
	bool bCanEverTick = true;
	USceneComponent* RootComponent = nullptr;
	bool bIsGizmo = false;

private:
	UWorld* World = nullptr;
	TSet<UActorComponent*> Components;
};

