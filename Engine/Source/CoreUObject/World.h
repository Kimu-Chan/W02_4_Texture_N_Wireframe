#pragma once

#include "Engine/Engine.h"
#include "CoreUObject/Object.h"
#include "CoreUObject/ObjectFactory.h"
#include "Core/Container/Array.h"
#include "Core/Container/Set.h"
#include "Core/Container/String.h"
#include "Debugging/DebugConsole.h"
#include "Utils/JsonSavehelper.h"

#include "Engine/GameFrameWork/Picker.h"

class AActor;

class UWorld :public UObject
{
public:
	UWorld() = default;
	virtual ~UWorld() = default;

public:
	void BeginPlay();
	void Tick(float DeltaTime);
	void LateTick(float DeltaTime);

	template <typename T>
		requires std::derived_from<T, AActor>
	T* SpawnActor(FString InName, const FVector& InLocation, const FVector& InRotation, const FVector& InScale, AActor* InOwner);
  
	bool DestroyActor(AActor* InActor);
	
	void Render();
	void RenderPickingTexture(URenderer& Renderer);
	void DisplayPickingTexture(URenderer& Renderer);
	void RenderMainTexture(URenderer& Renderer);

	void ClearWorld();
	void LoadWorld(const char* SceneName);
	void SaveWorld();

	void AddZIgnoreComponent(UPrimitiveComponent* InComponent);
	void RemoveZIgnoreComponent(UPrimitiveComponent* InComponent) {ZIgnoreRenderComponents.Remove(InComponent); }
	bool ContainsZIgnoreComponent(UPrimitiveComponent* InComponent) {return ZIgnoreRenderComponents.Find(InComponent) != -1; }
	
	// render
	void AddRenderComponent(class UPrimitiveComponent* Component) { RenderComponents.Add(Component); }
	void RemoveRenderComponent(class UPrimitiveComponent* Component) { RenderComponents.Remove(Component); }

private:
	UWorldInfo GetWorldInfo() const;

public:
	FString SceneName;
	uint32 Version = 1;
	
protected:
	TArray<AActor*> Actors;
	TArray<UPrimitiveComponent*> ZIgnoreRenderComponents;
	TArray<AActor*> ActorsToSpawn;
	TArray<AActor*> PendingDestroyActors; // TODO: 추후에 TQueue로 변경
	TSet<UPrimitiveComponent*> RenderComponents;
};

template <typename T>
	requires std::derived_from<T, AActor>
T* UWorld::SpawnActor(FString InName, const FVector& InLocation, const FVector& InRotation, const FVector& InScale, AActor* InOwner)
{
	T* NewActor = FObjectFactory::ConstructObject<T>();
	if (NewActor != nullptr)
	{
		// TODO: loc, rot, scale 세팅
		
	}
	if (UWorld* World = UEngine::Get().GetWorld())
	{
		Actors.Add(NewActor);
		ActorsToSpawn.Add(NewActor);
		return NewActor;
	}

	UE_LOG("Actor Construction Failed. World is nullptr");
	return nullptr;
}