#include "pch.h" 
#include "Actor.h"
#include "CoreUObject/World.h"
#include "CoreUObject/Components/SceneComponent.h"
#include "CoreUObject/Components/PrimitiveComponent.h"
#include "Debugging/DebugConsole.h"
#include "Static/EditorManager.h"

AActor::AActor() {}

void AActor::BeginPlay()
{
    for (auto& Component : Components)
    {
        Component->BeginPlay();

        if (UPrimitiveComponent* PrimitiveComponent = dynamic_cast<UPrimitiveComponent*>(Component))
        {
            PrimitiveComponent->RegisterComponentWithWorld(GetWorld());
        }
    }
}

void AActor::Tick(float DeltaTime)
{
    for (auto& Component : Components)
    {
        if (Component->CanEverTick())
        {
            Component->TickComponent(DeltaTime);
        }
    }
}

void AActor::LateTick(float DeltaTime)
{
}

void AActor::Destroyed()
{
    EndPlay(EEndPlayReason::Destroyed);
}

void AActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    for (auto& Component : Components)
    {
        Component->EndPlay(EndPlayReason);
        if (const auto PrimitiveComp = dynamic_cast<UPrimitiveComponent*>(Component))
        {
            if (GetWorld()->ContainsZIgnoreComponent(PrimitiveComp))
            {
                GetWorld()->RemoveZIgnoreComponent(PrimitiveComp);
            }

            GetWorld()->RemoveRenderComponent(PrimitiveComp);
        }
        if (FEditorManager::Get().GetSelectedActor() == this)
        {
            FEditorManager::Get().SelectActor(nullptr);
        }
        UEngine::Get().GObjects.Remove(Component->GetUUID());
    }
    Components.Empty();
}

UWorld* AActor::GetWorld()
{
    if (!World)
    {
        World = UEngine::Get().GetWorld();
    }
    return World;
}

FVector AActor::GetActorLocation() const
{
    if (RootComponent)
    {
        return RootComponent->GetRelativeTransform().GetLocation();
    }
    return FVector::ZeroVector;
}

FVector AActor::GetActorRotation() const
{
    if (RootComponent)
    {
        return RootComponent->GetRelativeTransform().GetRotation().GetEuler();
    }
    return FVector::ZeroVector;
}

FVector AActor::GetActorScale3D() const
{
    if (RootComponent)
    {
        return RootComponent->GetRelativeTransform().GetScale();
    }
    return FVector::OneVector;
}

bool AActor::SetActorLocation(const FVector& InLocation)
{
    if (RootComponent)
    {
        RootComponent->SetRelativeLocation(InLocation);
        return true;
    }
    return false;
}

bool AActor::SetActorRotation(const FVector& InRotation)
{
    if (RootComponent)
    {
        RootComponent->SetRelativeRotation(InRotation);
        return true;
    }
    return false;
}

bool AActor::SetActorScale3D(const FVector& InScale)
{
    if (RootComponent)
    {
        RootComponent->SetRelativeScale(InScale);
        return true;
    }
    return false;
}

void AActor::OnActorPicked()
{
    if (RootComponent)
    {
        bIsPicked = true;
        RootComponent->Pick(true);
    }
}

void AActor::OnActorUnPicked()
{
    if (RootComponent)
    {
        bIsPicked = false;
        RootComponent->Pick(false);
    }
}

FTransform AActor::GetActorTransform() const
{
    return RootComponent != nullptr ? RootComponent->GetRelativeTransform() : FTransform();
}

void AActor::SetActorTransform(const FTransform& InTransform)
{
    // InTransform�� ���� ������
    if (RootComponent)
    {
        RootComponent->SetRelativeTransform(InTransform);
    }
    else
    {
        UE_LOG(TEXT("RootComponent is nullptr"));
    }
}

const char* AActor::GetTypeName()
{
    return "Actor";
}

bool AActor::Destroy()
{
    return GetWorld()->DestroyActor(this);
}

void AActor::SetColor(FVector4 InColor)
{
    if (RootComponent == nullptr)
    {
        return;
    }

    if (UPrimitiveComponent* RootPrimitive = dynamic_cast<UPrimitiveComponent*>(RootComponent))
    {
        RootPrimitive->SetCustomColor(InColor);
    }

    for (auto& Component : Components)
    {
        if (UPrimitiveComponent* PrimitiveComponent = dynamic_cast<UPrimitiveComponent*>(Component))
        {
            PrimitiveComponent->SetCustomColor(InColor);
        }
    }
}

void AActor::SetUseVertexColor(bool bUseVertexColor)
{
    if (RootComponent == nullptr)
    {
        return;
    }

    if (UPrimitiveComponent* RootPrimitive = dynamic_cast<UPrimitiveComponent*>(RootComponent))
    {
        RootPrimitive->SetUseVertexColor(bUseVertexColor);
    }

    for (auto& Component : Components)
    {
        if (UPrimitiveComponent* PrimitiveComponent = dynamic_cast<UPrimitiveComponent*>(Component))
        {
            PrimitiveComponent->SetUseVertexColor(bUseVertexColor);
        }
    }
}
