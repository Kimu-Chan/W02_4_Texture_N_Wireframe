#include "pch.h"
#include "CatActor.h"

#include "Camera.h"
#include "EditorManager.h"
#include "Components/AnimatedBillboard.h"
#include "Components/Billboard.h"
#include "Rendering/TextureLoader.h"

REGISTER_CLASS(ACatActor)

ACatActor::ACatActor()
{
    UBillboard* Root = AddComponent<UBillboard>();
    RootComponent = Root;
    TextureInfo* TexInfo = UEngine::Get().GetTextureInfo(L"Cat");
    Root->SetTexture(TexInfo->ShaderResourceView);

    UAnimatedBillboard* HappyCat = AddComponent<UAnimatedBillboard>();
    TexInfo = UEngine::Get().GetTextureInfo(L"HappyCat");
    HappyCat->SetTexture(TexInfo->ShaderResourceView, 11, 11);
    HappyCat->SetupAttachment(Root);
    HappyCat->SetPlayRate(25);
    HappyCat->SetRelativeTransform(FTransform(FVector(0.f, 0.f, 2.f), FVector::ZeroVector, FVector(2.f, 2.f, 2.f)));

    UAnimatedBillboard* AppleCat = AddComponent<UAnimatedBillboard>();
    TexInfo = UEngine::Get().GetTextureInfo(L"AppleCat");
    AppleCat->SetTexture(TexInfo->ShaderResourceView, 2, 2);
    AppleCat->SetupAttachment(Root);
    AppleCat->SetPlayRate(12);
    AppleCat->SetRelativeTransform(FTransform(FVector(0.f, -2.f, 1.f), FVector::ZeroVector, FVector(1.f, 1.f, 1.f)));

    UAnimatedBillboard* DancingCat = AddComponent<UAnimatedBillboard>();
    TexInfo = UEngine::Get().GetTextureInfo(L"DancingCat");
    DancingCat->SetTexture(TexInfo->ShaderResourceView, 2, 2);
    DancingCat->SetupAttachment(Root);
    DancingCat->SetPlayRate(4);
    DancingCat->SetRelativeTransform(FTransform(FVector(0.f, 3.f, 0.f), FVector::ZeroVector, FVector(3.f, 3.f, 3.f)));
}

void ACatActor::BeginPlay()
{
    AActor::BeginPlay();

    
}

void ACatActor::Tick(float DeltaTime)
{
    AActor::Tick(DeltaTime);

    ACamera* Camera = FEditorManager::Get().GetCamera();
    FTransform ComponentTransform = GetActorTransform();
    ComponentTransform.LookAt(Camera->GetActorTransform().GetPosition());
    SetActorTransform(ComponentTransform);
}
