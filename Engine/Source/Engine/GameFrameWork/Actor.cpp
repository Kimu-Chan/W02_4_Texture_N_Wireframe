#include "pch.h" 
#include "Actor.h"
#include "CoreUObject/World.h"
#include "CoreUObject/Components/SceneComponent.h"
#include "CoreUObject/Components/PrimitiveComponent.h"
#include "Editor/Windows/ConsoleWindow.h"
#include "Static/EditorManager.h"
#include "Components/Billboard.h"
#include "Components/TextBillboard.h"
#include "Core/Rendering/TextureLoader.h"
#include "Camera.h"

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
			if(PrimitiveComponent->bCanBeRendered)
				PrimitiveComponent->RegisterComponentWithWorld(World);

			if (bUseBoundingBox)
			{
				PrimitiveComponent->InitBoundingBox();
				PrimitiveComponent->SetBoundingBoxRenderable(bRenderBoundingBox);
			}

		}
	}
	if(IsGizmoActor() == false)
		InitUUIDBillboard();
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

	if (UUIDBillboard)
	{
		FTransform BillboardTransform = UUIDBillboard->GetWorldTransform();
		FVector BillboardPosition = GetActorTransform().GetPosition() + FVector(0.f, 0.f, 1.f);
		BillboardTransform.SetPosition(BillboardPosition);
		BillboardTransform.LookAt(FEditorManager::Get().GetCamera()->GetActorTransform().GetPosition());
		UUIDBillboard->SetRelativeTransform(BillboardTransform);
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
		if (FEditorManager::Get().GetSelectedComponent() && FEditorManager::Get().GetSelectedComponent()->GetOwner() == this)
		{
			FEditorManager::Get().ClearSelectedComponent();
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

void AActor::InitUUIDBillboard()
{
	UUIDBillboard = AddComponent<UTextBillboard>();
	UUIDBillboard->SetTexture(UEngine::Get().GetTextureInfo(L"ASCII")->ShaderResourceView, 16.f, 16.f);
	UUIDBillboard->SetText(L"UUID:" + std::to_wstring(GetUUID()));
	UUIDBillboard->SetBoundingBoxRenderable(false);
	UUIDBillboard->BeginPlay();
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
