#pragma once

#include "Core/AbstractClass/Singleton.h"

class APlayerController : public TSingleton<APlayerController>
{
	// TODO: 카메라는 플레이어 컨트롤러가 가지고있어야 함.
	
public :
	APlayerController();

	void ProcessPlayerInput(float DeltaTime);

	void HandleCameraMovement(float DeltaTime);
	void HandleGizmoMovement(float DeltaTime);
	void SaveCameraProperties(class ACamera* Camera);

	float GetCurrentSpeed() const { return CurrentSpeed; }
	float GetMaxSpeed() const { return MaxSpeed; }
	float GetMinSpeed() const { return MinSpeed; }

	void SetCurrentSpeed(float InSpeed) { CurrentSpeed = InSpeed; }

	float GetMouseSensitivity() const { return MouseSensitivity; }
	void SetMouseSensitivity(float InSensitivity) { MouseSensitivity = InSensitivity;}

	// TODO: 함수랑 변수 이름 맘에 안듬
	bool IsUiInput() const { return bUiInput; }
	void SetIsUiInput(bool bInUiInput) { bUiInput = bInUiInput; }
	
protected:
	float CurrentSpeed;
	float MaxSpeed;
	float MinSpeed;

	float MouseSensitivity;

	bool bIsHandlingGizmo;

	bool bUiInput = false;
	bool bUiCaptured = false;
};