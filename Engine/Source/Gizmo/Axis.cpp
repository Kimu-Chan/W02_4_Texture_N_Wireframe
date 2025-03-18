#include "pch.h" 
#include "Axis.h"
#include "Engine/Engine.h"
#include "World.h"

AAxis::AAxis()
{
    bIsGizmo = true;
    bUseBoundingBox = false;
    bRenderBoundingBox = false;

    ULineComp* LineX = AddComponent<ULineComp>();
    FTransform XTransform = LineX->GetComponentTransform();
    XTransform.SetScale(FVector(1000.0f, 1.0f, 1.0f));
    XTransform.Rotate({0.0f, 0.0f, 0.0f});
    LineX->SetRelativeTransform(XTransform);
    LineX->SetCustomColor(FVector4(1.0f, 0.0f, 0.0f, 1.0f));

    FVector Euler = LineX->GetComponentTransform().GetRotation().GetEuler();

    RootComponent = LineX;

    ULineComp* LineY = AddComponent<ULineComp>();
    FTransform YTransform = LineY->GetComponentTransform();
    YTransform.SetScale(FVector(1000.0f, 1.0f, 1.0f));
    YTransform.Rotate({0.0f, 0.0f, 90.0f});
    LineY->SetRelativeTransform(YTransform);
    LineY->SetCustomColor(FVector4(0.0f, 1.0f, 0.0f, 1.0f));


    Euler = LineY->GetComponentTransform().GetRotation().GetEuler();

    ULineComp* LineZ = AddComponent<ULineComp>();
    FTransform ZTransform = LineZ->GetComponentTransform();
    ZTransform.SetScale(FVector(1000.0f, 1.0f, 1.0f));
    ZTransform.Rotate({0.0f, -90.0f, 0.0f});
    LineZ->SetRelativeTransform(ZTransform);
    LineZ->SetCustomColor(FVector4(0.0f, 0.0f, 1.0f, 1.0f));

    Euler = LineZ->GetComponentTransform().GetRotation().GetEuler();

    UEngine::Get().GetWorld()->AddZIgnoreComponent(LineX);
    UEngine::Get().GetWorld()->AddZIgnoreComponent(LineY);
    UEngine::Get().GetWorld()->AddZIgnoreComponent(LineZ);
} 

void AAxis::BeginPlay()
{
    Super::BeginPlay();
        
}

void AAxis::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

const char* AAxis::GetTypeName()
{
    return "Axis";
}
