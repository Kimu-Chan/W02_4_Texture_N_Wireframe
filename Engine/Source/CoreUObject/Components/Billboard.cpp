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

    Renderer->UpdateTextureConstantBuffer(GetWorldTransform().GetMatrix(), RenderCol/TotalCols, RenderRow/TotalRows, TotalCols, TotalRows);
    // 렌더링
    Renderer->RenderBillboard();
}

void UBillboard::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
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


