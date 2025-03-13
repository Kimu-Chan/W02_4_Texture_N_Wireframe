#include "pch.h" 
#include "SceneComponent.h"
#include "PrimitiveComponent.h"
#include "Debugging/DebugConsole.h"

void USceneComponent::BeginPlay()
{
    Super::BeginPlay();
}

void USceneComponent::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);
}

// 월드 트랜스폼 반환
const FTransform USceneComponent::GetWorldTransform()
{
    if (AttachParent)
    {
        // 부모가 있을 경우: 로컬 * 부모 월드
        return GetRelativeTransform() * AttachParent->GetWorldTransform();
    }
    return GetRelativeTransform();
}

void USceneComponent::SetRelativeTransform(const FTransform& InTransform)
{
    RelativeLocation = InTransform.GetLocation();
    RelativeRotation = InTransform.GetRotation().GetEuler();
    RelativeScale = InTransform.GetScale();
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
        AttachParent = InParent;
        InParent->Children.Add(this);
        // TODO: 아래 함수를 호출해야하는지 생각해봐야함.
        // ApplyParentWorldTransform(InParent->GetWorldTransform());
    }
    else
    {
        UE_LOG(TEXT("Parent is nullptr"));
    }
}

void USceneComponent::ApplyParentWorldTransform(const FTransform& ParentWorldTransform)
{
    FMatrix ParentWorld = ParentWorldTransform.GetMatrix();
    FMatrix MyLocal = GetRelativeTransform().GetMatrix();

    FMatrix NewMatrix = MyLocal * ParentWorld.Inverse();

    // 로컬 트랜스폼 갱신
    SetRelativeTransform(NewMatrix.GetTransform());
}
