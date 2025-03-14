#pragma once

#include "Actor.h"

class ACone : public AActor
{
	UCLASS(ACone, AActor);
    using Super = AActor;
public:
    ACone();
    virtual ~ACone() = default;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual const char* GetTypeName() override;
};

