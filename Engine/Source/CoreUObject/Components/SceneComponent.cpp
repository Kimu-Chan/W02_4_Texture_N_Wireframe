#include "pch.h" 
#include "SceneComponent.h"
#include "PrimitiveComponent.h"
#include "Debugging/DebugConsole.h"

void USceneComponent::BeginPlay()
{
    Super::BeginPlay();
}

void USceneComponent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// 월드 트랜스폼 반환
const FTransform USceneComponent::GetWorldTransform()
{
    if (Parent)
    {
            // 부모 월드 * 내 로컬
            FMatrix ParentWorld = Parent->GetWorldTransform().GetMatrix();
            FMatrix MyLocal = RelativeTransform.GetMatrix();

            FMatrix NewMatrix = MyLocal * ParentWorld;
            return NewMatrix.GetTransform();
    }

    return RelativeTransform;
}

void USceneComponent::SetRelativeTransform(const FTransform& InTransform)
{
    // 로컬 트랜스폼 갱신
    RelativeTransform = InTransform;
    FVector Rot = RelativeTransform.GetRotation().GetEuler();

}

void USceneComponent::Pick(bool bPicked)
{
    bIsPicked = bPicked;
    for (auto& Child : Children)
    {
            Child->Pick(bPicked);
    }
}

void USceneComponent::SetupAttachment(USceneComponent* InParent, bool bUpdateChildTransform)
{
    if (InParent)
    {
            Parent = InParent;
            InParent->Children.Add(this);
            ApplyParentWorldTransform(InParent->GetWorldTransform());
    }
    else
    {
            UE_LOG("Parent is nullptr");
    }
}

void USceneComponent::ApplyParentWorldTransform(const FTransform& ParentWorldTransform)
{
    FMatrix ParentWorld = ParentWorldTransform.GetMatrix();
    FMatrix MyLocal = RelativeTransform.GetMatrix();

    FMatrix NewMatrix = MyLocal * ParentWorld.Inverse();

    // 로컬 트랜스폼 갱신
    SetRelativeTransform(NewMatrix.GetTransform());
}
