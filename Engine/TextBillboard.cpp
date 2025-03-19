#include "pch.h"
#include "TextBillboard.h"
#include "Engine/GameFrameWork/Actor.h"
#include "CoreUObject/World.h"

UTextBillboard::UTextBillboard()
{
}

void UTextBillboard::BeginPlay()
{
	Super::BeginPlay();
	GetOwner()->GetWorld()->AddBillboardComponent(this);

	int32 VertexCount = TextString.size() * 6;
	UEngine::Get().GetRenderer()->CreateTextVertexBuffer(VertexCount);
}

void UTextBillboard::Render(class URenderer* Renderer)
{
	if (nullptr == Texture || TextString.empty())
	{
		return;
	}

	// World에서 Render를 위한 Prepare를 모두 하고 들어오기 때문에 
	// 새로 업데이트 되는 버텍스 버퍼와 텍스처를 셋팅해준다. (필요하면 상수버퍼도)

	Renderer->GetDeviceContext()->PSSetShaderResources(0, 1, &Texture);
	Renderer->UpdateTextConstantBuffer(GetWorldTransform().GetMatrix());
	Renderer->RenderTextBillboard(TextString, TotalCols, TotalRows);
}

void UTextBillboard::SetText(const std::wstring& InString)
{
	TextString = InString;
}


