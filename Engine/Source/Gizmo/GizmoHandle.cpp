#include "pch.h" 
#include "GizmoHandle.h"

#include "Engine/GameFrameWork/Camera.h"
#include "CoreUObject/Components/PrimitiveComponent.h"
#include "CoreUObject/World.h"
#include "Core/Input/PlayerInput.h"
#include "Static/EditorManager.h"

AGizmoHandle::AGizmoHandle()
{
	bIsGizmo = true;
	bUseBoundingBox = false;
	bRenderBoundingBox = false;

	// !NOTE : Z�������� ������
	// z
	UCylinderComp* ZArrow = AddComponent<UCylinderComp>();
	ZArrow->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(1, 1, 1)));
	ZArrow->SetCustomColor(FVector4(0.0f, 0.0f, 1.0f, 1.0f));
	CylinderComponents.Add(ZArrow);

	// x
	UCylinderComp* XArrow = AddComponent<UCylinderComp>();
	XArrow->SetupAttachment(ZArrow);
	XArrow->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, -90.0f, 0.0f), FVector(1, 1, 1)));
	XArrow->SetCustomColor(FVector4(1.0f, 0.0f, 0.0f, 1.0f));
	CylinderComponents.Add(XArrow);

	// y
	UCylinderComp* YArrow = AddComponent<UCylinderComp>();
	YArrow->SetupAttachment(ZArrow);
	YArrow->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(90.0f, 0.0f, 0.0f), FVector(1, 1, 1)));
	YArrow->SetCustomColor(FVector4(0.0f, 1.0f, 0.0f, 1.0f));
	CylinderComponents.Add(YArrow);
	RootComponent = ZArrow;

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

	if (SelectedAxis != ESelectedAxis::None)
	{
		if (AActor* Actor = FEditorManager::Get().GetSelectedActor())
		{
			// ���콺�� Ŀ�� ��ġ�� ��������
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(UEngine::Get().GetWindowHandle(), &pt);

			RECT Rect;
			GetClientRect(UEngine::Get().GetWindowHandle(), &Rect);
			int ScreenWidth = Rect.right - Rect.left;
			int ScreenHeight = Rect.bottom - Rect.top;

			// Ŀ�� ��ġ�� NDC�� ����
			float PosX = 2.0f * pt.x / ScreenWidth - 1.0f;
			float PosY = -2.0f * pt.y / ScreenHeight + 1.0f;

			// Projection �������� ��ȯ
			FVector4 RayOrigin{ PosX, PosY, 0.0f, 1.0f };
			FVector4 RayEnd{ PosX, PosY, 1.0f, 1.0f };

			// View �������� ��ȯ
			FMatrix InvProjMat = UEngine::Get().GetRenderer()->GetProjectionMatrix().Inverse();
			RayOrigin = InvProjMat.TransformVector4(RayOrigin);
			RayOrigin.W = 1;
			RayEnd = InvProjMat.TransformVector4(RayEnd);
			RayEnd *= 1000.0f;  // ���������� Far ���� ������ �ȵż� �������� ����
			RayEnd.W = 1;

			// ���콺 �������� ���� ��ġ�� ����
			FMatrix InvViewMat = FEditorManager::Get().GetCamera()->GetViewMatrix().Inverse();
			RayOrigin = InvViewMat.TransformVector4(RayOrigin);
			RayOrigin /= RayOrigin.W = 1;
			RayEnd = InvViewMat.TransformVector4(RayEnd);
			RayEnd /= RayEnd.W = 1;
			FVector RayDir = (RayEnd - RayOrigin).GetSafeNormal();

			// ���Ϳ��� �Ÿ�
			float Distance = FVector::Distance(RayOrigin, Actor->GetActorTransform().GetPosition());

			// Ray �������� Distance��ŭ ����
			FVector Result = RayOrigin + RayDir * Distance;

			FTransform AT = Actor->GetActorTransform();

			DoTransform(AT, Result, Actor);

		}
	}

	if (APlayerInput::Get().GetKeyDown(EKeyCode::Space))
	{
		int type = static_cast<int>(GizmoType);
		type = (type + 1) % static_cast<int>(EGizmoType::Max);
		GizmoType = static_cast<EGizmoType>(type);
	}

}

void AGizmoHandle::SetScaleByDistance()
{
	FTransform MyTransform = GetActorTransform();

	// ������ ���� ��ġ
	FVector actorWorldPos = MyTransform.GetPosition();

	FTransform CameraTransform = FEditorManager::Get().GetCamera()->GetActorTransform();

	// ī�޶��� ���� ��ġ
	FVector cameraWorldPos = CameraTransform.GetPosition();

	// �Ÿ� ���
	float distance = (actorWorldPos - cameraWorldPos).Length();

	float baseScale = 1.0f;    // �⺻ ������
	float scaleFactor = distance * 0.1f; // �Ÿ��� ����Ͽ� ������ ����

	// float minScale = 1.0f;     // �ּ� ������
	// float maxScale = 1.0f;     // �ִ� ������
	// float scaleFactor = clamp(1.0f / distance, minScale, maxScale);

	MyTransform.SetScale(scaleFactor, scaleFactor, scaleFactor);
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

void AGizmoHandle::DoTransform(FTransform& AT, FVector Result, AActor* Actor)
{
	const FVector& AP = AT.GetPosition();

	if (SelectedAxis == ESelectedAxis::X)
	{
		switch (GizmoType)
		{
		case EGizmoType::Translate:
			AT.SetPosition({ Result.X, AP.Y, AP.Z });
			break;
		case EGizmoType::Rotate:
			AT.RotatePitch(Result.X);
			break;
		case EGizmoType::Scale:
			AT.AddScale({ Result.X * .1f, 0, AP.Z * .1f });
			break;
		}
	}
	else if (SelectedAxis == ESelectedAxis::Y)
	{
		switch (GizmoType)
		{
		case EGizmoType::Translate:
			AT.SetPosition({ AP.X, Result.Y, AP.Z });
			break;
		case EGizmoType::Rotate:
			AT.RotateRoll(Result.Y);
			break;
		case EGizmoType::Scale:
			AT.AddScale({ 0, Result.Y * .1f, 0 });
			break;
		}
	}
	else if (SelectedAxis == ESelectedAxis::Z)
	{
		switch (GizmoType)
		{
		case EGizmoType::Translate:
			AT.SetPosition({ AP.X, AP.Y, Result.Z });
			break;
		case EGizmoType::Rotate:
			AT.RotatePitch(-Result.Z);
			break;
		case EGizmoType::Scale:
			AT.AddScale({ 0, 0, Result.Z * .1f });
			break;
		}
	}
	Actor->SetActorTransform(AT);
}

