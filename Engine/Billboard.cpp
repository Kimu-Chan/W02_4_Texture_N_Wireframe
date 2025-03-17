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

void UBillboard::Render()
{
    if (Texture == nullptr)
    {
        return;
    }

    URenderer* Renderer = UEngine::Get().GetRenderer();

    // 텍스처와 샘플러 상태를 셰이더에 설정
    Renderer->GetDeviceContext()->PSSetShaderResources(0, 1, &Texture);

    Renderer->UpdateTextureConstantBuffer(GetWorldTransform().GetMatrix(), 1.f, 1.f);
    // 렌더링
    Renderer->RenderBillboard();
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

void UBillboard::EndPlay(const EEndPlayReason::Type Reason)
{
    GetOwner()->GetWorld()->RemoveBillboardComponent(this);
}

void UBillboard::SetTexture(ID3D11ShaderResourceView* InTexture)
{
    Texture = InTexture;
}


