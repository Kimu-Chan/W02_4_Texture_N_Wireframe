#include "pch.h"
#include "AnimatedBillboard.h"

REGISTER_CLASS(UAnimatedBillboard)

UAnimatedBillboard::UAnimatedBillboard()
    : PlayRate(1.f)
    , RemainingNextFrameTime(1.f / PlayRate)
    , UvIndex(0)
{
    bCanBeRendered = true;
}

void UAnimatedBillboard::BeginPlay()
{
    Super::BeginPlay();
}

void UAnimatedBillboard::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    RemainingNextFrameTime -= DeltaTime;
    if (RemainingNextFrameTime <= 0)
    {
        RemainingNextFrameTime = 1.f / PlayRate;
        
        ++UvIndex;
        int32 MaxIndex = static_cast<int32>(TotalCols) * static_cast<int32>(TotalRows);
        UvIndex = UvIndex % MaxIndex;
        RenderRow = static_cast<int32>(UvIndex / TotalCols);
        RenderCol = static_cast<int32>(UvIndex % static_cast<int32>(TotalCols));
    }
}

void UAnimatedBillboard::Render(class URenderer* Renderer)
{
    Super::Render(Renderer);
}
