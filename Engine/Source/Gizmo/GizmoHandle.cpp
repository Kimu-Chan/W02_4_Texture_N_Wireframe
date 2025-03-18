#include "pch.h" 
#include "GizmoHandle.h"

#include "Engine/GameFrameWork/Camera.h"
#include "CoreUObject/Components/PrimitiveComponent.h"
#include "CoreUObject/World.h"
#include "Core/Input/PlayerInput.h"
#include "Static/EditorManager.h"

REGISTER_CLASS(AGizmoHandle);
AGizmoHandle::AGizmoHandle()
{
	bIsGizmo = true;
	bUseBoundingBox = true;
	bRenderBoundingBox = false;

	// !NOTE : Z방향으로 서있음
	// z
	UCylinderComp* ZArrow = AddComponent<UCylinderComp>();
	ZArrow->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(1, 1, 1)));
	ZArrow->SetCustomColor(FVector4(0.0f, 0.0f, 1.0f, 1.0f));
	CylinderComponents.Add(ZArrow);

    // x
    UCylinderComp* XArrow = AddComponent<UCylinderComp>();
    XArrow->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 90.0f, 0.0f), FVector(1, 1, 1)));
    XArrow->SetCustomColor(FVector4(1.0f, 0.0f, 0.0f, 1.0f));
    CylinderComponents.Add(XArrow);

    // y
    UCylinderComp* YArrow = AddComponent<UCylinderComp>();
    YArrow->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(-90.0f, 0.0f, 0.0f), FVector(1, 1, 1)));
    YArrow->SetCustomColor(FVector4(0.0f, 1.0f, 0.0f, 1.0f));
    CylinderComponents.Add(YArrow);
    RootComponent = ZArrow;

    XArrow->SetupAttachment(ZArrow);
    YArrow->SetupAttachment(ZArrow);
    UEngine::Get().GetWorld()->AddZIgnoreComponent(ZArrow);
    UEngine::Get().GetWorld()->AddZIgnoreComponent(XArrow);
    UEngine::Get().GetWorld()->AddZIgnoreComponent(YArrow);

    SetActive(false);
}

void AGizmoHandle::Tick(float DeltaTime)
{
    AActor* SelectedActor = FEditorManager::Get().GetSelectedActor();
    if (SelectedActor != nullptr && bIsActive)
    {
        FTransform GizmoTr = RootComponent->GetComponentTransform();
        GizmoTr.SetPosition(SelectedActor->GetActorTransform().GetPosition());
        SetActorTransform(GizmoTr);
    }

    SetScaleByDistance();

    AActor::Tick(DeltaTime);

	if (APlayerInput::Get().IsMouseReleased(false))
	{
		SelectedAxis = ESelectedAxis::None;
		CachedRayResult = FVector::ZeroVector;
	}
    if (SelectedAxis != ESelectedAxis::None)
    {
        if (AActor* Actor = FEditorManager::Get().GetSelectedActor())
        {
            // 마우스의 커서 위치를 가져오기
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(UEngine::Get().GetWindowHandle(), &pt);

            RECT Rect;
            GetClientRect(UEngine::Get().GetWindowHandle(), &Rect);
            int ScreenWidth = Rect.right - Rect.left;
            int ScreenHeight = Rect.bottom - Rect.top;

			float PosX = 2.0f * pt.x / ScreenWidth - 1.0f;
			float PosY = -2.0f * pt.y / ScreenHeight + 1.0f;

			FVector4 RayOrigin{ PosX, PosY, 0.0f, 1.0f };
			FVector4 RayEnd{ PosX, PosY, 1.0f, 1.0f };

			FMatrix InvProjMat = UEngine::Get().GetRenderer()->GetProjectionMatrix().Inverse();
			RayOrigin = InvProjMat.TransformVector4(RayOrigin);
			RayOrigin.W = 1;
			RayEnd = InvProjMat.TransformVector4(RayEnd);
			RayEnd *= 1000.0f;  
			RayEnd.W = 1;

			FMatrix InvViewMat = FEditorManager::Get().GetCamera()->GetViewMatrix().Inverse();
			RayOrigin = InvViewMat.TransformVector4(RayOrigin);
			RayOrigin /= RayOrigin.W = 1;
			RayEnd = InvViewMat.TransformVector4(RayEnd);
			RayEnd /= RayEnd.W = 1;
			FVector RayDir = (RayEnd - RayOrigin).GetSafeNormal();

			float Distance = FVector::Distance(RayOrigin, Actor->GetActorTransform().GetPosition());

            // 이전 프레임의 Result가 있어야 함
			FVector Result = RayOrigin + RayDir * Distance;

			if (CachedRayResult == FVector::ZeroVector)
			{
				CachedRayResult = Result;
                return;
			}

			FVector Delta = Result - CachedRayResult;
            CachedRayResult = Result;
            FTransform AT = Actor->GetActorTransform();

			DoTransform(AT, Delta, Actor);
		}
        else
        {
			CachedRayResult = FVector::ZeroVector;
        }
	}

    if (APlayerInput::Get().IsKeyPressed(DirectX::Keyboard::Keys::Space))
    {
        int type = static_cast<int>(GizmoType);
        type = (type + 1) % static_cast<int>(EGizmoType::Max);
        GizmoType = static_cast<EGizmoType>(type);
    }

}

void AGizmoHandle::SetScaleByDistance()
{
    FTransform MyTransform = GetActorTransform();

    // 액터의 월드 위치
    FVector actorWorldPos = MyTransform.GetPosition();

    FTransform CameraTransform = FEditorManager::Get().GetCamera()->GetActorTransform();

    // 카메라의 월드 위치
    FVector cameraWorldPos = CameraTransform.GetPosition();

    // 거리 계산
    float distance = (actorWorldPos - cameraWorldPos).Length();

    float baseScale = 3.0f;    // 기본 스케일
    float scaleFactor = distance * (1.f/baseScale); // 거리에 비례하여 스케일 증d가

    // float minScale = 1.0f;     // 최소 스케일
    // float maxScale = 1.0f;     // 최대 스케일
    // float scaleFactor = clamp(1.0f / distance, minScale, maxScale);

    MyTransform.SetScale(scaleFactor, scaleFactor, scaleFactor);
    SetActorTransform(MyTransform);
}

void AGizmoHandle::SetActive(bool bActive)
{
    bIsActive = bActive;
    for (auto& Cylinder : CylinderComponents)
    {
        Cylinder->SetCanBeRendered(bActive);
    }
}

const char* AGizmoHandle::GetTypeName()
{
    return "GizmoHandle";
}

void AGizmoHandle::DoTransform(FTransform& ActorTransform, FVector Delta, AActor* Actor)
{
    const FVector& Position = ActorTransform.GetPosition();

    if (SelectedAxis == ESelectedAxis::X)
    {
        switch (GizmoType)
        {
        case EGizmoType::Translate:
            ActorTransform.SetPosition({ Position.X + Delta.X, Position.Y, Position.Z });
            break;
        case EGizmoType::Rotate:
            ActorTransform.RotateRoll(Delta.X * 10.f);
            break;
        case EGizmoType::Scale:
            ActorTransform.AddScale({ Delta.X, 0, 0});
            break;
        }
    }
    else if (SelectedAxis == ESelectedAxis::Y)
    {
        switch (GizmoType)
        {
        case EGizmoType::Translate:
            ActorTransform.SetPosition({ Position.X, Position.Y + Delta.Y, Position.Z });
            break;
        case EGizmoType::Rotate:
            ActorTransform.RotatePitch(Delta.Y * 10.f);
            break;
        case EGizmoType::Scale:
            ActorTransform.AddScale({ 0, Delta.Y, 0 });
            break;
        }
    }
    else if (SelectedAxis == ESelectedAxis::Z)
    {
        switch (GizmoType)
        {
        case EGizmoType::Translate:
            ActorTransform.SetPosition({ Position.X, Position.Y, Position.Z + Delta.Z });
            break;
        case EGizmoType::Rotate:
            ActorTransform.RotateYaw(Delta.Z * 10.f);
            break;
        case EGizmoType::Scale:
            ActorTransform.AddScale({ 0, 0, Delta.Z });
            break;
        }
    }
    Actor->SetActorTransform(ActorTransform);
}

