#include "pch.h" 
#include "Actor.h"
#include "CoreUObject/World.h"
#include "CoreUObject/Components/SceneComponent.h"
#include "CoreUObject/Components/PrimitiveComponent.h"
#include "Debugging/DebugConsole.h"
#include "Static/EditorManager.h"

REGISTER_CLASS(AActor);
AActor::AActor() : Depth{ 0 }
{
}

void AActor::BeginPlay()
{
	for (auto& Component : Components)
	{
		Component->BeginPlay();

		if (UPrimitiveComponent* PrimitiveComponent = dynamic_cast<UPrimitiveComponent*>(Component))
		{
			PrimitiveComponent->RegisterComponentWithWorld(World);

			if (bUseBoundingBox)
			{
				PrimitiveComponent->InitBoundingBox();
				PrimitiveComponent->SetBoundingBoxRenderable(bRenderBoundingBox);
			}

		}
	}
}

void AActor::Tick(float DeltaTime)
{
	for (auto& Component : Components)
	{
		if (Component->CanEverTick())
		{
			Component->Tick(DeltaTime);
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
			if (World->ContainsZIgnoreComponent(PrimitiveComp))
			{
				World->RemoveZIgnoreComponent(PrimitiveComp);
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

void AActor::SetBoundingBoxRenderable(bool bRenderable)
{
	for (UActorComponent* Component : Components)
	{
		if (UPrimitiveComponent* PrimitiveComponent = dynamic_cast<UPrimitiveComponent*>(Component))
		{
			PrimitiveComponent->SetBoundingBoxRenderable(bRenderable);
		}
	}
}

void AActor::Pick()
{
	if (RootComponent)
	{
		bIsPicked = true;
		RootComponent->Pick(true);

		SetBoundingBoxRenderable(true);
	}
}

void AActor::UnPick()
{
	if (RootComponent)
	{
		bIsPicked = false;
		RootComponent->Pick(false);

		SetBoundingBoxRenderable(false);

	}
}

FTransform AActor::GetActorTransform() const
{
	return RootComponent != nullptr ? RootComponent->GetComponentTransform() : FTransform();
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
		UE_LOG("RootComponent is nullptr");
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

	UPrimitiveComponent* RootPrimitive = dynamic_cast<UPrimitiveComponent*>(RootComponent);
	if (RootPrimitive)
	{
		RootPrimitive->SetCustomColor(InColor);
	}

	for (auto& Component : Components)
	{
		UPrimitiveComponent* PrimitiveComponent = dynamic_cast<UPrimitiveComponent*>(Component);
		if (PrimitiveComponent)
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

	UPrimitiveComponent* RootPrimitive = dynamic_cast<UPrimitiveComponent*>(RootComponent);
	if (RootPrimitive)
	{
		RootPrimitive->SetUseVertexColor(bUseVertexColor);
	}

	for (auto& Component : Components)
	{
		UPrimitiveComponent* PrimitiveComponent = dynamic_cast<UPrimitiveComponent*>(Component);
		if (PrimitiveComponent)
		{
			PrimitiveComponent->SetUseVertexColor(bUseVertexColor);
		}
	}
}
