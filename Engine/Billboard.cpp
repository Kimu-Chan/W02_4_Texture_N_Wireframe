#include "pch.h"
#include "Billboard.h"

REGISTER_CLASS(UBillboard);
UBillboard::UBillboard()
	: Texture(nullptr), SamplerState(nullptr)
{
    // 샘플러 상태 생성
    D3D11_SAMPLER_DESC SamplerDesc = {};
    SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    SamplerDesc.MinLOD = 0;
    SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    URenderer* Renderer = UEngine::Get().GetRenderer();
    Renderer->GetDevice()->CreateSamplerState(&SamplerDesc, &SamplerState);
}

void UBillboard::Render()
{
    if (Texture == nullptr)
    {
        return;
    }

    URenderer* Renderer = UEngine::Get().GetRenderer();
    Renderer->PrepareTexture();

    // 텍스처와 샘플러 상태를 셰이더에 설정
    Renderer->GetDeviceContext()->PSSetShaderResources(0, 1, &Texture);
    Renderer->GetDeviceContext()->PSSetSamplers(0, 1, &SamplerState);

    Renderer->UpdateTextureConstantBuffer(GetWorldTransform().GetMatrix(), 1, 1);

    // 렌더링
    Renderer->RenderTexture();
}

void UBillboard::SetTexture(ID3D11ShaderResourceView* InTexture)
{
    Texture = InTexture;
}