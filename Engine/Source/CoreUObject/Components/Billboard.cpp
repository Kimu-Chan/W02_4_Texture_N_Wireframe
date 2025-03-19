#include "pch.h"
#include "Billboard.h"
#include "Static/EditorManager.h"
#include "Engine/GameFrameWork/Camera.h"
#include "World.h"

REGISTER_CLASS(UBillboard);
UBillboard::UBillboard()
	: Texture(nullptr)
{
	bCanBeRendered = false;
}

void UBillboard::BeginPlay()
{
    Super::BeginPlay();
	GetOwner()->GetWorld()->AddBillboardComponent(this);
}

void UBillboard::Render(class URenderer* Renderer)
{
    if (Renderer == nullptr || Texture == nullptr)
    {
        return;
    }

    // 텍스처와 샘플러 상태를 셰이더에 설정
    Renderer->GetDeviceContext()->PSSetShaderResources(0, 1, &Texture);

    Renderer->UpdateTextureConstantBuffer(GetWorldTransform().GetMatrix(), RenderCol/TotalCols, RenderRow/TotalRows, TotalCols, TotalRows, PartyHsvToRgb(PartyHue));
    // 렌더링
    Renderer->RenderBillboard();
}

void UBillboard::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    PartyHue += (DeltaTime * 500.f);
    PartyHue = fmod(PartyHue, 360.0f);
    if (PartyHue < 0)
        PartyHue += 360.0f;
}

void UBillboard::EndPlay(const EEndPlayReason::Type Reason)
{
    GetOwner()->GetWorld()->RemoveBillboardComponent(this);
}

void UBillboard::SetTexture(ID3D11ShaderResourceView* InTexture, float Cols, float Rows)
{
    Texture = InTexture;
    TotalCols = Cols;
    TotalRows = Rows;
}

void UBillboard::SetRenderUV(float Col, float Row)
{
	RenderCol = Col;
	RenderRow = Row;
}

FVector4 UBillboard::PartyHsvToRgb(float Hue)
{
    Hue = fmod(Hue, 360.0f);
    if (Hue < 0) Hue += 360.0f;

    float h = Hue / 60.0f;  // sector 0 to 5
    int i = static_cast<int>(h);
    float f = h - i;        // fractional part
    // Since s and v are 100% (i.e., 1.0), p, q, t are calculated as follows:
    float p = 0.0f;
    float q = 1.0f - f;
    float t = f;

    float r, g, b;
    switch (i) {
    case 0: r = 1.0f; g = t;    b = p;    break;
    case 1: r = q;    g = 1.0f; b = p;    break;
    case 2: r = p;    g = 1.0f; b = t;    break;
    case 3: r = p;    g = q;    b = 1.0f; break;
    case 4: r = t;    g = p;    b = 1.0f; break;
    case 5: r = 1.0f; g = p;    b = q;    break;
    default: r = 1.0f; g = 0.0f; b = 0.0f; break; // Fallback
    }
    return FVector4(r, g, b, 1.0f);
}


