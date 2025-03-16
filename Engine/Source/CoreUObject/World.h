#pragma once

#include "Engine/Engine.h"
#include "CoreUObject/Object.h"
#include "CoreUObject/ObjectFactory.h"
#include "Core/Container/Array.h"
#include "Core/Container/Set.h"
#include "Core/Container/String.h"
#include "Debugging/DebugConsole.h"
#include "Utils/JsonSavehelper.h"

#include "Engine/GameFrameWork/Arrow.h"
#include "Engine/GameFrameWork/Picker.h"
#include "Math/Ray.h"


class AActor;

class UWorld :public UObject
{
	UCLASS(UWorld, UObject);
	using Super = UObject;
public:
	UWorld() = default;
	virtual ~UWorld() = default;

public:
	void BeginPlay();
	void Tick(float DeltaTime);
	void LateTick(float DeltaTime);

	template <typename T>
		requires std::derived_from<T, AActor>
	T* SpawnActor();
  
	bool DestroyActor(AActor* InActor);
	
	void Render(float DeltaTime);
	void RenderPickingTexture(URenderer& Renderer);
	void DisplayPickingTexture(URenderer& Renderer);
	void RenderMainTexture(URenderer& Renderer);
	void RenderBoundingBoxes(URenderer& Renderer);
	void RenderWorldGrid(URenderer& Renderer);
	void RenderDebugLines(URenderer& Renderer, float DeltaTime);

	void ClearWorld();
	void LoadWorld(const char* SceneName);
	void SaveWorld();

	void AddZIgnoreComponent(UPrimitiveComponent* InComponent);
	void RemoveZIgnoreComponent(UPrimitiveComponent* InComponent) {ZIgnoreRenderComponents.Remove(InComponent); }
	bool ContainsZIgnoreComponent(UPrimitiveComponent* InComponent) {return ZIgnoreRenderComponents.Find(InComponent) != -1; }
	
	// render
	void AddRenderComponent(class UPrimitiveComponent* Component) { RenderComponents.Add(Component); }
	void RemoveRenderComponent(class UPrimitiveComponent* Component) { RenderComponents.Remove(Component); }

	TArray<AActor*> GetActors() const { return Actors; };
	
private:
	UWorldInfo GetWorldInfo() const;

public:
	// BoundingBox & Linetrace
	bool LineTrace(const FRay& Ray, USceneComponent** FirstHitComponent) const;
	void AddBoundingBox(FBox* Box) { BoundingBoxes.Add(Box); }
	void RemoveBoundingBox(FBox* Box) { BoundingBoxes.Remove(Box); }

	void DrawDebugLine(FVector Start, FVector End, FVector Color, float Time) const;
	bool IsDebuggingRaycast() const { return bDebugRaycast; }
	void SetDebugRaycast(bool bInDebugRaycast) { bDebugRaycast = bInDebugRaycast; }

private:
	bool bDebugRaycast = false;
	
public:
	FString SceneName;
	uint32 Version = 1;
	
protected:
	TArray<AActor*> Actors;
	TArray<UPrimitiveComponent*> ZIgnoreRenderComponents;
	TArray<AActor*> ActorsToSpawn;
	TArray<AActor*> PendingDestroyActors; // TODO: 추후에 TQueue로 변경
	TSet<UPrimitiveComponent*> RenderComponents;
	TSet<class FBox*> BoundingBoxes;

	TSet<FString> ActorNames;
};

template <typename T>
	requires std::derived_from<T, AActor>
T* UWorld::SpawnActor()
{
	T* Actor = FObjectFactory::ConstructObject<T>();

	UWorld* World = UEngine::Get().GetWorld();
	if (!World)
	{
		UE_LOG("Actor Construction Failed. World is nullptr");
		return nullptr;
	}

	Actor->SetWorld(World);
	Actors.Add(Actor);
	//ActorsToSpawn.Add(Actor);

	FString NewActorName = Actor->GetTypeName();
	if (ActorNames.Contains(NewActorName))
	{
		uint32 Count = 0;
		NewActorName += "_";
		while (Count < UINT_MAX)
		{
			FString NumToStr = FString(std::to_string(Count));
			FString TempName = NewActorName;
			TempName += NumToStr;
			if (!ActorNames.Contains(TempName))
			{
				Actor->SetName(TempName);
				ActorNames.Add(TempName);
				break;
			}
			++Count;
		}

		if (Count == UINT_MAX)
		{
			// TODO: 어떤 동작을 해야할지 고민해봐야 함.
		}
	}
	else
	{
		Actor->SetName(NewActorName);
		ActorNames.Add(NewActorName);
	}
	
	Actor->BeginPlay();
	
	return Actor;
}
