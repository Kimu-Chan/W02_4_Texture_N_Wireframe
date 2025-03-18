#pragma once
#include "PrimitiveComponent.h"

class UMeshComponent : public UPrimitiveComponent
{
    UCLASS(USceneComponent, UMeshComponent);
    using Super = UPrimitiveComponent;
public:
    UMeshComponent() = default;
    
public:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void Render(URenderer* Renderer) override;

    void SetMeshName(FName InMeshName) { MeshName = InMeshName; }
    FName GetMeshName() const { return MeshName; }
    
private:
    FName MeshName;
    
};
