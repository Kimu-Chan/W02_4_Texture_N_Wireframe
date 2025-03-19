#pragma once
#include "Billboard.h"

class UAnimatedBillboard : public UBillboard
{
    UCLASS(UAnimatedBillboard, UBillboard)
    using Super = UBillboard;
    
public:
    UAnimatedBillboard();
    virtual ~UAnimatedBillboard() = default;

    virtual void BeginPlay() override;
    
    virtual void Tick(float DeltaTime) override;

    virtual void Render(class URenderer* Renderer) override;

    void SetPlayRate(int32 InPlayRate) { PlayRate = InPlayRate; }

    float PartyTrigger = 0.f;

protected:
    virtual	FVector4 PartyHsvToRgb(float Hue) override;

private:
    int32 PlayRate; // FPS

    float RemainingNextFrameTime;

    int32 UvIndex;

    float AccumulatedTime = 0.f;
};
