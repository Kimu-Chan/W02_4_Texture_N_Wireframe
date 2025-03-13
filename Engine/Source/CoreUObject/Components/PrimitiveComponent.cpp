#include "pch.h" 
#include "PrimitiveComponent.h"
#include "CoreUObject/World.h"
#include "Engine/GameFrameWork/Actor.h"


void UPrimitiveComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UPrimitiveComponent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime); 
}

void UPrimitiveComponent::UpdateConstantPicking(const URenderer& Renderer, const FVector4 UUIDColor)const
{
    Renderer.UpdateConstantPicking(UUIDColor);
}

void UPrimitiveComponent::UpdateConstantDepth(const URenderer& Renderer, const int Depth)const
{
    Renderer.UpdateConstantDepth(Depth);
}

void UPrimitiveComponent::Render()
{
    URenderer* Renderer = UEngine::Get().GetRenderer();
    if (Renderer == nullptr || !bCanBeRendered)
    {
        return;
    }
    if (GetOwner()->IsGizmoActor() == false)
    {
        if (bIsPicked)
        {
            /*bUseVertexColor = false;
            SetCustomColor(FVector4(1.0f, 0.647f, 0.0f, 1.0f));*/
        }
        else
        {
            bUseVertexColor = true;
        }
    }
    Renderer->RenderPrimitive(this);
}

void UPrimitiveComponent::RegisterComponentWithWorld(UWorld* World)
{
    World->AddRenderComponent(this);
}
