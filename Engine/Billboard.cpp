#include "pch.h"
#include "Billboard.h"
#include "Static/EditorManager.h"
#include "Engine/GameFrameWork/Camera.h"

REGISTER_CLASS(UBillboard);
UBillboard::UBillboard()
	: Texture(nullptr), SamplerState(nullptr)
{
	bCanBeRendered = true;

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

    // 텍스처와 샘플러 상태를 셰이더에 설정
    Renderer->GetDeviceContext()->PSSetShaderResources(0, 1, &Texture);
    Renderer->GetDeviceContext()->PSSetSamplers(0, 1, &SamplerState);

    Renderer->UpdateTextureConstantBuffer(GetWorldTransform().GetMatrix(), 1.f, 1.f);

    // 렌더링
    Renderer->RenderTexture();
}

void UBillboard::Tick(float DeltaTime)
{
    FVector Rotation = RelativeTransform.GetRotation().GetEuler();
    //UE_LOG("Billboard Position: %f, %f, %f", RelativeTransform.GetPosition().X, RelativeTransform.GetPosition().Y, RelativeTransform.GetPosition().Z);
    //UE_LOG("Billboard Scale: %f, %f", RelativeTransform.GetScale().X, RelativeTransform.GetScale().Y, );
    //UE_LOG("Billboard Rotation: %f, %f, %f", Rotation.X, Rotation.Y, Rotation.Z);
    
    Super::Tick(DeltaTime);

	FVector Position = FEditorManager::Get().GetCamera()->GetActorTransform().GetPosition();
    RelativeTransform.LookAt(Position);
}

void UBillboard::SetTexture(ID3D11ShaderResourceView* InTexture)
{
    Texture = InTexture;
}

