#pragma once

#include "Actor.h"

class ACylinder : public AActor
{
	UCLASS(ACylinder, AActor);
    using Super = AActor;
public:
    ACylinder();
    virtual ~ACylinder() = default;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual const char* GetTypeName() override;
};

