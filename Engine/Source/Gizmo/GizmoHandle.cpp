#include "pch.h" 
#include "GizmoHandle.h"

#include "Engine/GameFrameWork/Camera.h"
#include "CoreUObject/Components/PrimitiveComponent.h"
#include "CoreUObject/World.h"
#include "Core/Input/PlayerInput.h"
#include "Static/EditorManager.h"
#include "Components/MeshComponent.h"

REGISTER_CLASS(AGizmoHandle);
AGizmoHandle::AGizmoHandle()
{
	bIsGizmo = true;
	bUseBoundingBox = true;
	bRenderBoundingBox = false;

    USceneComponent* Root = AddComponent<USceneComponent>();
    RootComponent = Root;

    InitTranslationGizmo();
    InitRotationGizmo();
    InitScaleGizmo();

    OnGizmoTypeChanged(GizmoType);
    
    SetActive(false);
}

void AGizmoHandle::InitTranslationGizmo()
{
    // x
    UMeshComponent* TranslationX = AddComponent<UMeshComponent>();
    TranslationX->SetMeshName("GizmoTranslation");
    TranslationX->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), GizmoScale));
    TranslationX->SetCustomColor(FVector4(1.0f, 0.0f, 0.0f, 1.0f));
    AllGizmos.Add(TranslationX);
    TranslationGizmos.Add(TranslationX);
    TranslationX->SetupAttachment(RootComponent);

    // y
    UMeshComponent* TranslationY = AddComponent<UMeshComponent>();
    TranslationY->SetMeshName("GizmoTranslation");
    TranslationY->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 90.0f), GizmoScale));
    TranslationY->SetCustomColor(FVector4(0.0f, 1.0f, 0.0f, 1.0f));
    AllGizmos.Add(TranslationY);
    TranslationGizmos.Add(TranslationY);
    TranslationY->SetupAttachment(RootComponent);

    // z
    UMeshComponent* TranslationZ = AddComponent<UMeshComponent>();
    TranslationZ->SetMeshName("GizmoTranslation");
    TranslationZ->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, -90.0f, 0.0f), GizmoScale));
    TranslationZ->SetCustomColor(FVector4(0.0f, 0.0f, 1.0f, 1.0f));
    AllGizmos.Add(TranslationZ);
    TranslationGizmos.Add(TranslationZ);
    TranslationZ->SetupAttachment(RootComponent);

    UEngine::Get().GetWorld()->AddZIgnoreComponent(TranslationX);
    UEngine::Get().GetWorld()->AddZIgnoreComponent(TranslationY);
    UEngine::Get().GetWorld()->AddZIgnoreComponent(TranslationZ);
}

void AGizmoHandle::InitRotationGizmo()
{
    // x
    UMeshComponent* RotationX = AddComponent<UMeshComponent>();
    RotationX->SetMeshName("GizmoRotation");
    RotationX->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), GizmoScale));
    RotationX->SetCustomColor(FVector4(1.0f, 0.0f, 0.0f, 1.0f));
    AllGizmos.Add(RotationX);
    RotationGizmos.Add(RotationX);
    RotationX->SetupAttachment(RootComponent);

    // y
    UMeshComponent* RotationY = AddComponent<UMeshComponent>();
    RotationY->SetMeshName("GizmoRotation");
    RotationY->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(90.0f, 0.0f, 90.0f), GizmoScale));
    RotationY->SetCustomColor(FVector4(0.0f, 1.0f, 0.0f, 1.0f));
    AllGizmos.Add(RotationY);
    RotationGizmos.Add(RotationY);
    RotationY->SetupAttachment(RootComponent);

    // z
    UMeshComponent* RotationZ = AddComponent<UMeshComponent>();
    RotationZ->SetMeshName("GizmoRotation");
    RotationZ->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, -90.0f, -90.0f), GizmoScale));
    RotationZ->SetCustomColor(FVector4(0.0f, 0.0f, 1.0f, 1.0f));
    AllGizmos.Add(RotationZ);
    RotationGizmos.Add(RotationZ);
    RotationZ->SetupAttachment(RootComponent);

    UEngine::Get().GetWorld()->AddZIgnoreComponent(RotationZ);
    UEngine::Get().GetWorld()->AddZIgnoreComponent(RotationX);
    UEngine::Get().GetWorld()->AddZIgnoreComponent(RotationY);
}

void AGizmoHandle::InitScaleGizmo()
{
    // x
    UMeshComponent* ScaleX = AddComponent<UMeshComponent>();
    ScaleX->SetMeshName("GizmoScale");
    ScaleX->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), GizmoScale));
    ScaleX->SetCustomColor(FVector4(1.0f, 0.0f, 0.0f, 1.0f));
    AllGizmos.Add(ScaleX);
    ScaleGizmos.Add(ScaleX);
    ScaleX->SetupAttachment(RootComponent);

    // y
    UMeshComponent* ScaleY = AddComponent<UMeshComponent>();
    ScaleY->SetMeshName("GizmoScale");
    ScaleY->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 90.0f), GizmoScale));
    ScaleY->SetCustomColor(FVector4(0.0f, 1.0f, 0.0f, 1.0f));
    AllGizmos.Add(ScaleY);
    ScaleGizmos.Add(ScaleY);
    ScaleY->SetupAttachment(RootComponent);

    // z
    UMeshComponent* ScaleZ = AddComponent<UMeshComponent>();
    ScaleZ->SetMeshName("GizmoScale");
    ScaleZ->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, -90.0f, 0.0f), GizmoScale));
    ScaleZ->SetCustomColor(FVector4(0.0f, 0.0f, 1.0f, 1.0f));
    AllGizmos.Add(ScaleZ);
    ScaleGizmos.Add(ScaleZ);
    ScaleZ->SetupAttachment(RootComponent);

    UEngine::Get().GetWorld()->AddZIgnoreComponent(ScaleZ);
    UEngine::Get().GetWorld()->AddZIgnoreComponent(ScaleX);
    UEngine::Get().GetWorld()->AddZIgnoreComponent(ScaleY);
}

void AGizmoHandle::OnGizmoTypeChanged(EGizmoType NewGizmoType)
{
    HideAllGizmo();

    switch (NewGizmoType)
    {
    case EGizmoType::Translate:
        for (auto& Gizmo : TranslationGizmos)
        {
            Gizmo->SetCanBeRendered(true);
        }
        break;
    case EGizmoType::Rotate:
        for (auto& Gizmo : RotationGizmos)
        {
            Gizmo->SetCanBeRendered(true);
        }
        break;
    case EGizmoType::Scale:
        for (auto& Gizmo : ScaleGizmos)
        {
            Gizmo->SetCanBeRendered(true);
        }
        break;
    default:
        break;
    }
}

void AGizmoHandle::HideAllGizmo()
{
    for (auto& Gizmo : AllGizmos)
    {
        Gizmo->SetCanBeRendered(false);
    }
}

void AGizmoHandle::Tick(float DeltaTime)
{
	USceneComponent* SelectedComponent = FEditorManager::Get().GetSelectedComponent();
    if (SelectedComponent != nullptr && bIsActive)
    {
        FTransform GizmoTr = RootComponent->GetComponentTransform();
        GizmoTr.SetPosition(SelectedComponent->GetWorldTransform().GetPosition());
		if (bIsLocal)
		{
            GizmoTr.SetRotation(SelectedComponent->GetWorldTransform().GetRotation());
		}
        else
        {
			GizmoTr.SetRotation(FQuat());
        }
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
        if (USceneComponent* SceneComp = FEditorManager::Get().GetSelectedComponent())
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

			float Distance = FVector::Distance(RayOrigin, SceneComp->GetComponentTransform().GetPosition());

            // 이전 프레임의 Result가 있어야 함
			FVector Result = RayOrigin + RayDir * Distance;

			if (CachedRayResult == FVector::ZeroVector)
			{
				CachedRayResult = Result;
                return;
			}

			FVector Delta = Result - CachedRayResult;
            CachedRayResult = Result;
            FTransform CompTransform = SceneComp->GetComponentTransform();

			DoTransform(CompTransform, Delta, SceneComp);
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
        OnGizmoTypeChanged(GizmoType);
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
    if (bIsActive)
    {
        OnGizmoTypeChanged(GizmoType);
    }
    else
    {
        HideAllGizmo();
    }
}

const char* AGizmoHandle::GetTypeName()
{
    return "GizmoHandle";
}

void AGizmoHandle::DoTransform(FTransform& CompTransform, FVector Delta, USceneComponent* SceneComp)
{
    FVector WorldDirection;
    FVector LocalDirection;

	FVector CamToComp = (SceneComp->GetComponentTransform().GetPosition() - FEditorManager::Get().GetCamera()->GetActorTransform().GetPosition()).GetSafeNormal();
	FVector RotationDelta = FVector::CrossProduct(Delta, CamToComp);



	float DeltaLength = Delta.Length();
    switch (SelectedAxis)
    {
	case ESelectedAxis::X:
		WorldDirection = FVector(1, 0, 0);
        LocalDirection = CompTransform.GetForward();
		break;
	case ESelectedAxis::Y:
		WorldDirection = FVector(0, 1, 0);
        LocalDirection = CompTransform.GetRight();
		break;
	case ESelectedAxis::Z:
		WorldDirection = FVector(0, 0, 1);
		LocalDirection = CompTransform.GetUp();
        break;
    }

	bool bIsLocal = FEditorManager::Get().GetGizmoHandle()->bIsLocal;
	FVector Direction = bIsLocal ? LocalDirection : WorldDirection;

	Delta = Direction * FVector::DotProduct(Delta, Direction);
	RotationDelta = Direction * FVector::DotProduct(RotationDelta, Direction);
	switch (GizmoType)
	{
	case EGizmoType::Translate:
		CompTransform.Translate(Delta);
		break;
	case EGizmoType::Rotate:
		CompTransform.Rotate(RotationDelta * 100);
		break;
	case EGizmoType::Scale:
        // 스케일은 축에 평행한 방향으로 커지는게 맞음
		Delta = WorldDirection * FVector::DotProduct(Delta, WorldDirection) * 2;
		CompTransform.AddScale(Delta);
		break;
	}
	SceneComp->SetRelativeTransform(CompTransform);
}

