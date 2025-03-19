#pragma once
#include "Actor.h"
#include "Components/AnimatedBillboard.h"

class ACatActor : public AActor
{
    UCLASS(ACatActor, AActor);
public:
    ACatActor();
    ~ACatActor() = default;

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    UAnimatedBillboard* HappyCatBillboard;
    UAnimatedBillboard* AppleCatBillboard;
    UAnimatedBillboard* DancingCatBillboard;
    
    float HappyCatTrigger = 2.f;
    float AppleCatTrigger = 2.5f;
    float DancingCatTrigger = 3.f;

    float AccumulatedTime = 0.f;
};
