#include "pch.h" 
#include "World.h"

#include "WorldGrid.h"
#include "Core/Input/PlayerInput.h"
#include "CoreUObject/Components/PrimitiveComponent.h"
#include "Gizmo/GizmoHandle.h"
#include "Engine/GameFrameWork/Camera.h"
#include "Static/EditorManager.h"
#include "Core/Container/Map.h"
#include "Utils/JsonSavehelper.h"

#include "Engine/GameFrameWork/Cone.h"
#include "Engine/GameFrameWork/Cube.h"
#include "Engine/GameFrameWork/Cylinder.h"
#include "Engine/GameFrameWork/Sphere.h"


void UWorld::BeginPlay()
{
    for (const auto& Actor : Actors)
    {
        Actor->BeginPlay();
    }
}

void UWorld::Tick(float DeltaTime)
{
    for (const auto& Actor : ActorsToSpawn)
    {
        Actor->BeginPlay();
    }
    ActorsToSpawn.Empty();

    const auto CopyActors = Actors;
    for (const auto& Actor : CopyActors)
    {
        if (Actor->CanEverTick())
        {
            Actor->Tick(DeltaTime);
        }
    }
}

void UWorld::LateTick(float DeltaTime)
{
    const auto CopyActors = Actors;
    for (const auto& Actor : CopyActors)
    {
        if (Actor->CanEverTick())
        {
            Actor->LateTick(DeltaTime);
        }
    }

    for (const auto& PendingActor : PendingDestroyActors)
    {
        // Remove from Engine
        UEngine::Get().GObjects.Remove(PendingActor->GetUUID());
    }
    PendingDestroyActors.Empty();
}

void UWorld::Render()
{
    URenderer* Renderer = UEngine::Get().GetRenderer();

    if (Renderer == nullptr)
    {
        return;
    }

    ACamera* cam = FEditorManager::Get().GetCamera();
    Renderer->UpdateViewMatrix(cam->GetActorTransform());
    Renderer->UpdateProjectionMatrix(cam);
        
    if (APlayerInput::Get().GetMouseDown(false))
    {
        RenderPickingTexture(*Renderer);
    }
        
    RenderMainTexture(*Renderer);
	RenderBoundingBoxes(*Renderer);

    RenderWorldGrid(*Renderer);
        
    // DisplayPickingTexture(*Renderer);

}

void UWorld::RenderPickingTexture(URenderer& Renderer)
{
    Renderer.PreparePicking();
    Renderer.PreparePickingShader();

    for (auto& RenderComponent : RenderComponents)
    {
        if (RenderComponent->GetOwner()->GetDepth() > 0)
        {
                continue;
        }
        uint32 UUID = RenderComponent->GetUUID();
        RenderComponent->UpdateConstantPicking(Renderer, APicker::EncodeUUID(UUID));
        RenderComponent->Render();
    }

    Renderer.PrepareZIgnore();
    for (auto& RenderComponent: ZIgnoreRenderComponents)
    {
        uint32 UUID = RenderComponent->GetUUID();
        RenderComponent->UpdateConstantPicking(Renderer, APicker::EncodeUUID(UUID));
        uint32 depth = RenderComponent->GetOwner()->GetDepth();
        RenderComponent->Render();
    }
}

void UWorld::RenderMainTexture(URenderer& Renderer)
{
    Renderer.PrepareMain();
    Renderer.PrepareMainShader();
    for (auto& RenderComponent : RenderComponents)
    {
        if (RenderComponent->GetOwner()->GetDepth() > 0)
        {
                continue;
        }
        uint32 depth = RenderComponent->GetOwner()->GetDepth();
        // RenderComponent->UpdateConstantDepth(Renderer, depth);
        RenderComponent->Render();
    }

    Renderer.PrepareZIgnore();
    for (auto& RenderComponent: ZIgnoreRenderComponents)
    {
        uint32 depth = RenderComponent->GetOwner()->GetDepth();
        RenderComponent->Render();
    }
}

void UWorld::RenderBoundingBoxes(URenderer& Renderer)
{
    for (FBox* Box : BoundingBoxes)
    {
        if (Box && Box->bCanBeRendered && Box->IsValidBox())
            Renderer.RenderBox(*Box);
    }
}

void UWorld::RenderWorldGrid(URenderer& Renderer)
{
    Renderer.RenderWorldGrid();
}

void UWorld::DisplayPickingTexture(URenderer& Renderer)
{
    Renderer.RenderPickingTexture();
}

void UWorld::ClearWorld()
{
    TArray CopyActors = Actors;
    for (AActor* Actor : CopyActors)
    {
        if (!Actor->IsGizmoActor())
        {
                DestroyActor(Actor);
        }
    }

    UE_LOG("Clear World");
}


bool UWorld::DestroyActor(AActor* InActor)
{
    //@TODO: 나중에 Destroy가 실패할 일이 있다면 return false; 하기
    assert(InActor);

    if (PendingDestroyActors.Find(InActor) != -1)
    {
        return true;
    }

    // 삭제될 때 Destroyed 호출
    InActor->Destroyed();

    // World에서 제거
    Actors.Remove(InActor);

    // 제거 대기열에 추가
    PendingDestroyActors.Add(InActor);
    return true;
}

void UWorld::SaveWorld()
{
    const UWorldInfo& WorldInfo = GetWorldInfo();
    JsonSaveHelper::SaveScene(WorldInfo);
}

void UWorld::AddZIgnoreComponent(UPrimitiveComponent* InComponent)
{
    ZIgnoreRenderComponents.Add(InComponent);
    InComponent->SetIsOrthoGraphic(true);
}

void UWorld::LoadWorld(const char* SceneName)
{
    if (SceneName == nullptr || strcmp(SceneName, "") == 0){
        return;
    }
        
    UWorldInfo* WorldInfo = JsonSaveHelper::LoadScene(SceneName);
    if (WorldInfo == nullptr) return;

    ClearWorld();

    Version = WorldInfo->Version;
    this->SceneName = WorldInfo->SceneName;
    uint32 ActorCount = WorldInfo->ActorCount;

    // Check Type
    for (uint32 i = 0; i < ActorCount; i++)
    {
        UObjectInfo* ObjectInfo = WorldInfo->ObjctInfos[i];
        FTransform Transform = FTransform(ObjectInfo->Location, FQuat(), ObjectInfo->Scale);
        Transform.Rotate(ObjectInfo->Rotation);

        AActor* Actor = nullptr;
                
        if (ObjectInfo->ObjectType == "Actor")
        {
            Actor = SpawnActor<AActor>();
        }
        else if (ObjectInfo->ObjectType == "Sphere")
        {
            Actor = SpawnActor<ASphere>();
        }
        else if (ObjectInfo->ObjectType == "Cube")
        {
            Actor = SpawnActor<ACube>();
        }
        else if (ObjectInfo->ObjectType == "Arrow")
        {
            Actor = SpawnActor<AArrow>();
        }
        else if (ObjectInfo->ObjectType == "Cylinder")
        {
            Actor = SpawnActor<ACylinder>();
        }
        else if (ObjectInfo->ObjectType == "Cone")
        {
            Actor = SpawnActor<ACone>();
        }
                
        Actor->SetActorTransform(Transform);
    }
}

UWorldInfo UWorld::GetWorldInfo() const
{
    UWorldInfo WorldInfo;
    WorldInfo.ActorCount = Actors.Num();
    WorldInfo.ObjctInfos = new UObjectInfo*[WorldInfo.ActorCount];
    WorldInfo.SceneName = *SceneName;
    WorldInfo.Version = 1;
    uint32 i = 0;
    for (auto& actor : Actors)
    {
        if (actor->IsGizmoActor())
        {
            WorldInfo.ActorCount--;
            continue;
        }
        WorldInfo.ObjctInfos[i] = new UObjectInfo();
        const FTransform& Transform = actor->GetActorTransform();
        WorldInfo.ObjctInfos[i]->Location = Transform.GetPosition();
        WorldInfo.ObjctInfos[i]->Rotation = Transform.GetRotation();
        WorldInfo.ObjctInfos[i]->Scale = Transform.GetScale();
        WorldInfo.ObjctInfos[i]->ObjectType = actor->GetTypeName();

        WorldInfo.ObjctInfos[i]->UUID = actor->GetUUID();
        i++;
    }
    return WorldInfo;
}

bool UWorld::LineTrace(const FRay& Ray, USceneComponent** FirstHitComponent) const
{
	for (FBox* Box : BoundingBoxes)
	{
		if (Box && Box->IsValidBox() && Box->IntersectRay(Ray))
		{
			*FirstHitComponent = Box->GetOwner();
			return true;
		}
	}
    return false;
}
