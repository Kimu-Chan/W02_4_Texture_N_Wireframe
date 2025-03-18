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
}

void UTextBillboard::Render()
{
	if (TextString.empty())
	{
		Super::Render();
		return;
	}
	UpdateTextVertexBuffer();

	ID3D11DeviceContext* DeviceContext = UEngine::Get().GetRenderer()->GetDeviceContext();
}

void UTextBillboard::SetText(const std::wstring& InString)
{
	TextString = InString;
}

void UTextBillboard::CreateTextVertexBuffer(int32 InVertexCount)
{
	if (TextVertexBuffer)
	{
		TextVertexBuffer->Release();
		TextVertexBuffer = nullptr;
	}

	D3D11_BUFFER_DESC Desc = {};
	Desc.Usage = D3D11_USAGE_DYNAMIC;
	Desc.ByteWidth = sizeof(FVertexUV) * InVertexCount;
	Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT HR = UEngine::Get().GetRenderer()->GetDevice()->CreateBuffer(&Desc, nullptr, &TextVertexBuffer);
	if (HR != S_OK)
	{
		//UE_LOG(L"Failed to create Text Vertex Buffer");
	}
}

void UTextBillboard::UpdateTextVertexBuffer()
{
	int32 VertexCount = TextString.size() * 6;
	CreateTextVertexBuffer(VertexCount);

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	ID3D11DeviceContext* DeviceContext = UEngine::Get().GetRenderer()->GetDeviceContext();

	if (SUCCEEDED(DeviceContext->Map(TextVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource)))
	{
		FVertexUV* Vertices = reinterpret_cast<FVertexUV*>(MappedResource.pData);

		float StartX = 0.f;
		float StartY = 0.f;
		float CharWidth = 32.f;
		float CharHeight = 32.f;

		int32 Index = 0;
		for (wchar_t ch : TextString)
		{
			int charIndex = ch;
			float U0 = (float)(charIndex % (int)TotalCols) / TotalCols;
			float V0 = (float)(charIndex / (int)TotalCols) / TotalRows;
			float U1 = U0 + 1.f / TotalCols;
			float V1 = V0 + 1.f / TotalRows;

			Vertices[Index++] = FVertexUV(StartX, StartY, 0.f, U0, V0);								// 왼쪽 위
			Vertices[Index++] = FVertexUV(StartX + CharWidth, StartY, 0.f, U1, V0);					// 오른쪽 위
			Vertices[Index++] = FVertexUV(StartX, StartY + CharHeight, 0.f, U0, V1);				// 왼쪽 아래
			Vertices[Index++] = FVertexUV(StartX + CharWidth, StartY, 0.f, U1, V0);					// 오른쪽 위
			Vertices[Index++] = FVertexUV(StartX + CharWidth, StartY + CharHeight, 0.f, U1, V1);	// 오른쪽 아래
			Vertices[Index++] = FVertexUV(StartX, StartY + CharHeight, 0.f, U0, V1);				// 왼쪽 아래
			
			StartX += CharWidth;
		}
		DeviceContext->Unmap(TextVertexBuffer, 0);
	}
}
