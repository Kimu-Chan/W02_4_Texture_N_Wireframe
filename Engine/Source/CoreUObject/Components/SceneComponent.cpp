#include "pch.h" 
#include "SceneComponent.h"
#include "PrimitiveComponent.h"
#include "Debugging/DebugConsole.h"
#include "Core/Math/Vector.h"
#include "Core/Math/Matrix.h"
#include "Core/Math/Box.h"
#include "GameFrameWork/Actor.h"
#include "World.h"


REGISTER_CLASS(USceneComponent);
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
        //FMatrix ParentWorld = Parent->GetWorldTransform().GetMatrix();

        //FMatrix MyLocal = RelativeTransform.GetMatrix();

        //FMatrix NewMatrix = MyLocal * ParentWorld;
        return Parent->GetWorldTransform() * RelativeTransform;
    }

    return RelativeTransform;
}

void USceneComponent::SetRelativeTransform(const FTransform& InTransform)
{
    // 로컬 트랜스폼 갱신
    RelativeTransform = InTransform;
    UpdateBoundingBox();
}

void USceneComponent::SetWorldTransform(const FTransform& InTransform)
{
	// !TODO 구현 필요
	//if (Parent)
	//{
 //       FTransform ParentInverse = Parent->GetWorldTransform().Inverse();
 //       RelativeTransform = ParentInverse * InTransform;
	//}
	//else
	//{
	//	RelativeTransform = InTransform;
	//}
    UpdateBoundingBox();
}

void USceneComponent::Pick(bool bPicked)
{
    bIsPicked = bPicked;
    for (auto& Child : Children)
    {
        Child->Pick(bPicked);
    }
}

void USceneComponent::EndPlay(const EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);
    if (BoundingBox)
    {
        GetOwner()->GetWorld()->RemoveBoundingBox(BoundingBox.get());
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
    //FMatrix ParentWorld = ParentWorldTransform.GetMatrix();
    //FMatrix MyLocal = RelativeTransform.GetMatrix();

    //FMatrix NewMatrix = MyLocal * ParentWorld.Inverse();

    // 로컬 트랜스폼 갱신
    SetRelativeTransform(ParentWorldTransform * RelativeTransform);
    UpdateBoundingBox();

}

void USceneComponent::UpdateBoundingBox()
{
    if (BoundingBox)
    {
        BoundingBox->Update(GetWorldTransform().GetMatrix());
    }
    for (USceneComponent* Child : Children)
    {
        Child->UpdateBoundingBox();
    }
}
