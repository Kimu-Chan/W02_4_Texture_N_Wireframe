#pragma once

#include "Engine/GameFrameWork/Actor.h"

enum class ESelectedAxis : uint8
{
	None,
	X,
	Y,
	Z
};

enum class EGizmoType : uint8
{
	Translate,
	Rotate,
	Scale,
	Max
};

class AGizmoHandle : public AActor
{
	UCLASS(AGizmoHandle, AActor);
	using Super = AActor;
public:
	AGizmoHandle();

public:
	virtual void Tick(float DeltaTime) override;
	void SetScaleByDistance();
	void SetActive(bool bActive);
	void SetSelectedAxis(ESelectedAxis NewAxis) { SelectedAxis = NewAxis; }
	ESelectedAxis GetSelectedAxis() const { return SelectedAxis; }
	EGizmoType GetGizmoType() const { return GizmoType; }
	void SetLocal(bool bIsLocal) { this->bIsLocal = bIsLocal; }

private:
	bool bIsActive = false;
	TArray<class UCylinderComp*> CylinderComponents;

	ESelectedAxis SelectedAxis = ESelectedAxis::None;
	EGizmoType GizmoType = EGizmoType::Translate;

public:
	virtual const char* GetTypeName() override;

	bool bIsLocal = false;
private:
	void DoTransform(FTransform& AT, FVector Result, AActor* Actor);
	FVector CachedRayResult = FVector::ZeroVector;
};

