#include "pch.h"
#include "MeshComponent.h"

REGISTER_CLASS(UMeshComponent);
void UMeshComponent::BeginPlay()
{
    Super::BeginPlay();

    
}

void UMeshComponent::Tick(float DeltaTime)
{
    UPrimitiveComponent::Tick(DeltaTime);
}

void UMeshComponent::Render(URenderer* Renderer)
{
    if (Renderer == nullptr || !bCanBeRendered)
    {
        return;
    }
    Renderer->RenderMesh(this);
}
