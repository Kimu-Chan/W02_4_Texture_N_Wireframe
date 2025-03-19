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
    Root->bCanBeRendered = true;

    HappyCatBillboard = AddComponent<UAnimatedBillboard>();
    TexInfo = UEngine::Get().GetTextureInfo(L"HappyCat"); // TODO: TEXT 매크로
    HappyCatBillboard->SetTexture(TexInfo->ShaderResourceView, 11, 11);
    HappyCatBillboard->SetupAttachment(Root);
    HappyCatBillboard->SetPlayRate(25);
    HappyCatBillboard->SetRelativeTransform(FTransform(FVector(0.f, 0.f, 2.f), FVector::ZeroVector, FVector(2.f, 2.f, 2.f)));
    HappyCatBillboard->bCanBeRendered = false;
    HappyCatBillboard->PartyTrigger = 3.5f;
    
    AppleCatBillboard = AddComponent<UAnimatedBillboard>();
    TexInfo = UEngine::Get().GetTextureInfo(L"AppleCat");
    AppleCatBillboard->SetTexture(TexInfo->ShaderResourceView, 2, 2);
    AppleCatBillboard->SetupAttachment(Root);
    AppleCatBillboard->SetPlayRate(12);
    AppleCatBillboard->SetRelativeTransform(FTransform(FVector(0.f, -2.f, 1.f), FVector::ZeroVector, FVector(1.f, 1.f, 1.f)));
    AppleCatBillboard->bCanBeRendered = false;
    AppleCatBillboard->PartyTrigger = 3.5f;
    
    DancingCatBillboard = AddComponent<UAnimatedBillboard>();
    TexInfo = UEngine::Get().GetTextureInfo(L"DancingCat");
    DancingCatBillboard->SetTexture(TexInfo->ShaderResourceView, 2, 2);
    DancingCatBillboard->SetupAttachment(Root);
    DancingCatBillboard->SetPlayRate(4);
    DancingCatBillboard->SetRelativeTransform(FTransform(FVector(0.f, 3.f, 0.f), FVector::ZeroVector, FVector(3.f, 3.f, 3.f)));
    DancingCatBillboard->bCanBeRendered = false;
    DancingCatBillboard->PartyTrigger = 3.5f;
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

    if (AccumulatedTime < DancingCatTrigger)
    {
        AccumulatedTime += DeltaTime;
        if (AccumulatedTime >= HappyCatTrigger)
        {
            HappyCatBillboard->bCanBeRendered = true;
        }
        if (AccumulatedTime >= AppleCatTrigger)
        {
            AppleCatBillboard->bCanBeRendered = true;
        }
        if (AccumulatedTime >= DancingCatTrigger)
        {
            DancingCatBillboard->bCanBeRendered = true;
        }
    }
}

const char* ACatActor::GetTypeName()
{
    return "CatActor";
}