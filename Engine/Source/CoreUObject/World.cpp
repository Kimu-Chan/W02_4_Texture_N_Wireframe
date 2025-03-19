#include "pch.h" 
#include "World.h"

#include "WorldGrid.h"
#include "Core/Input/PlayerInput.h"
#include "CoreUObject/Components/PrimitiveComponent.h"
#include "Components/MeshComponent.h"
#include "Engine/GameFrameWork/Camera.h"
#include "Core/Container/Map.h"
#include "Utils/JsonSavehelper.h"

#include "Engine/GameFrameWork/Cone.h"
#include "Engine/GameFrameWork/Cube.h"
#include "Engine/GameFrameWork/Cylinder.h"
#include "Engine/GameFrameWork/Sphere.h"
#include "Input/PlayerController.h"

#include "Components/Billboard.h"

REGISTER_CLASS(UWorld);
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

void UWorld::Render(float DeltaTime)
{
    URenderer* Renderer = UEngine::Get().GetRenderer();

    if (Renderer == nullptr)
    {
        return;
    }


    /**
     * Axis는 Grid에 가려지면 안되므로 Grid 먼저 렌더.
     * Axis는 아래의 RenderMainTexture 함수에서 렌더됨.
     */
    RenderWorldGrid(*Renderer);
        
    if (!APlayerController::Get().IsUiInput() && APlayerInput::Get().IsMousePressed(false))
    {
        RenderPickingTexture(*Renderer);
    }
    RenderMainTexture(*Renderer);
    RenderMesh(*Renderer);
    
	RenderBoundingBoxes(*Renderer);
    RenderDebugLines(*Renderer, DeltaTime);
	RenderBillboard(*Renderer);

    // DisplayPickingTexture(*Renderer);
}

void UWorld::RenderPickingTexture(URenderer& Renderer)
{
    Renderer.PreparePicking();
    Renderer.PreparePickingShader();

    for (auto& RenderComponent : RenderComponents)
    {
        uint32 UUID = RenderComponent->GetUUID();
        RenderComponent->UpdateConstantPicking(Renderer, APicker::EncodeUUID(UUID));
        RenderComponent->Render(&Renderer);
    }

    Renderer.PrepareZIgnore();
    for (auto& RenderComponent: ZIgnoreRenderComponents)
    {
        uint32 UUID = RenderComponent->GetUUID();
        RenderComponent->UpdateConstantPicking(Renderer, APicker::EncodeUUID(UUID));
        RenderComponent->Render(&Renderer);
    }
}

void UWorld::RenderMainTexture(URenderer& Renderer)
{
    Renderer.PrepareMain();
    Renderer.PrepareMainShader();

    bool bRenderPrimitives = UEngine::Get().GetShowPrimitives();
    for (auto& RenderComponent : RenderComponents)
    {
        if (RenderComponent->IsA<UMeshComponent>())
        {
            continue;
        }
        if (!bRenderPrimitives && !RenderComponent->GetOwner()->IsGizmoActor())
        {
            continue;
        }
        RenderComponent->Render(&Renderer);
    }

    Renderer.PrepareZIgnore();
    for (auto& RenderComponent: ZIgnoreRenderComponents)
    {
        if (RenderComponent->IsA<UMeshComponent>())
        {
            continue;
        }
        RenderComponent->Render(&Renderer);
    }
}

void UWorld::RenderMesh(URenderer& Renderer)
{
    Renderer.PrepareMesh();
    Renderer.PrepareMeshShader();
    
    bool bRenderPrimitives = UEngine::Get().GetShowPrimitives();
    for (auto& RenderComponent : RenderComponents)
    {
        if (!RenderComponent->IsA<UMeshComponent>())
        {
            continue;
        }
        if (!bRenderPrimitives && !RenderComponent->GetOwner()->IsGizmoActor())
        {
            continue;
        }
        RenderComponent->Render(&Renderer);
    }

    Renderer.PrepareZIgnore();
    for (auto& RenderComponent: ZIgnoreRenderComponents)
    {
        if (!RenderComponent->IsA<UMeshComponent>())
        {
            continue;
        }
        RenderComponent->Render(&Renderer);
    }
}

void UWorld::RenderBoundingBoxes(URenderer& Renderer)
{
	Renderer.PrepareMain();
	Renderer.PrepareMainShader();
    for (FBox* Box : BoundingBoxes)
    {
        if (Box && Box->bCanBeRendered && Box->IsValidBox())
        {
            Renderer.RenderBox(*Box);
        }
    }
}

void UWorld::RenderWorldGrid(URenderer& Renderer)
{
    Renderer.RenderWorldGrid();
}

void UWorld::RenderDebugLines(URenderer& Renderer, float DeltaTime)
{
    Renderer.RenderDebugLines(DeltaTime);
}

void UWorld::RenderBillboard(URenderer& Renderer)
{
    // 텍스처와 샘플러 상태를 셰이더에 설정
    Renderer.PrepareBillboard();

	for (UBillboard* Billboard : BillboardComponents)
	{
        if (Billboard)
        {
            Billboard->Render(&Renderer);
        }
	}
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

	// 삭제하고자 하는 Actor를 가지고 있는 ActorTreeNode를 찾아서 삭제
	for (ActorTreeNode* Node : ActorTreeNodes)
	{
		if (Node->GetActor() == InActor)
		{
            Node->GetParent()->RemoveChild(Node);
			for (ActorTreeNode* Child : Node->GetChildren())
			{
				Child->SetParent(Node->GetParent());
			}
			ActorTreeNodes.Remove(Node);
			break;
		}
	}

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
        if (Actor)
        {
            Actor->SetActorTransform(Transform);
        }
    }
}

UWorldInfo UWorld::GetWorldInfo() const
{
    UWorldInfo WorldInfo;
    WorldInfo.ActorCount = Actors.Num();
    WorldInfo.ObjctInfos = new UObjectInfo*[WorldInfo.ActorCount];
    WorldInfo.SceneName = std::string(SceneName.c_char());
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
        WorldInfo.ObjctInfos[i]->Rotation = Transform.GetRotation(); // TODO: GetRotation()의 리턴 타입은 FQuat으로, FVector로 변환된다는 보장 없음.
        WorldInfo.ObjctInfos[i]->Scale = Transform.GetScale();
        WorldInfo.ObjctInfos[i]->ObjectType = actor->GetTypeName();

        WorldInfo.ObjctInfos[i]->UUID = actor->GetUUID();
        i++;
    }
    return WorldInfo;
}

bool UWorld::LineTrace(const FRay& Ray, USceneComponent** FirstHitComponent) const
{
    TArray<TPair<USceneComponent*, float>> Hits;
	for (FBox* Box : BoundingBoxes)
	{
	    float Distance = 0.f;
		if (Box && Box->IsValidBox() && Box->IntersectRay(Ray, Distance))
		{
			Hits.Add({Box->GetOwner(), Distance});
		}
	}
    if (Hits.Num() == 0)
    {
        if (bDebugRaycast)
        {
            FVector Start = Ray.Origin;
            FVector End = Ray.Origin + Ray.Direction * Ray.Length;
            DrawDebugLine(Start, End, FVector(1.f, 0.f, 0.f), 10.f);
        }
        return false;
    }

    // Find min dist
    float MinDistance = FLT_MAX;
    for (const auto& [SceneComp, Dist] : Hits)
    {
        if (Dist < MinDistance)
        {
            MinDistance = Dist;
            *FirstHitComponent = SceneComp;
        }
    }
    if (bDebugRaycast)
    {
        FVector Start = Ray.Origin;
        FVector End = Ray.Origin + Ray.Direction * MinDistance;
        DrawDebugLine(Start, End, FVector(1.f, 0.f, 0.f), 5.f);
        
        Start = End;
        End = Start + Ray.Direction * (Ray.Length - MinDistance);
        DrawDebugLine(Start, End, FVector(0.f, 1.f, 0.f), 5.f);
    }
    
    return true;
}

void UWorld::DrawDebugLine(FVector Start, FVector End, FVector Color, float Time) const
{
    if (URenderer* Renderer = UEngine::Get().GetRenderer())
    {
        Renderer->AddDebugLine(Start, End, Color, Time);
    }
}
