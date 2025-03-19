#pragma once
#include "Actor.h"

class ACatActor : public AActor
{
    UCLASS(ACatActor, AActor);
public:
    ACatActor();
    ~ACatActor() = default;

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    
};
