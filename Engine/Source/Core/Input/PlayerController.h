#pragma once

#include "Core/AbstractClass/Singleton.h"

class APlayerController : public TSingleton<APlayerController> {
	public :
	APlayerController();

	void ProcessPlayerInput(float DeltaTime);

	void HandleCameraMovement(float DeltaTime);
	void HandleGizmoMovement(float DeltaTime);
	void SaveCameraProperties(class ACamera* Camera);
};