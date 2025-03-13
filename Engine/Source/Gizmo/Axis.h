#pragma once

#include "Engine/GameFrameWork/Actor.h"

/*
* UE의 InteractiveToolsFramework에 대하여 공부해볼 것
* @TODO: UObject를 상속받는 UGizmo 클래스로 변경
*/ 
class AAxis : public AActor
{
	using Super = AActor;
public:
	AAxis();
	virtual ~AAxis() = default;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual const char* GetTypeName() override;
};

