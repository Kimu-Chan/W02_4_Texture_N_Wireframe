#include "pch.h"
#include "MeshComponent.h"

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
    Renderer->RenderMesh(this);
}
