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
    
};
