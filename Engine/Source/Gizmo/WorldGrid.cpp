#include "pch.h"
#include "WorldGrid.h"

AWorldGrid::AWorldGrid()
{
    RootComponent = AddComponent<USceneComponent>(); // 위치와 스케일 정보만 가지고 있으면 충분하므로, Primitive가 아님.
    RootComponent->SetRelativeTransform(FTransform());
}

void AWorldGrid::Tick(float DeltaTime)
{
    AActor::Tick(DeltaTime);

}
