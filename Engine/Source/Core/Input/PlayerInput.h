#pragma once

#include "Core/AbstractClass/Singleton.h"
#include "Core/Math/Vector.h"

#include "DirectXTK/Keyboard.h"
#include "DirectXTK/Mouse.h"
#include "DirectXTK/GamePad.h"

class APlayerInput : public TSingleton<APlayerInput>
{
public:
	APlayerInput();

	void SetWindowSize(uint32 InWidth, uint32 InHeight);

	void UpdateInput();

	/**
	 * 키가 눌려있는지 확인.
	 * @param InKey 감지 할 키
	 * @return key의 눌림 여부
	 */
	bool IsPressedKey(DirectX::Keyboard::Keys InKey) const;

	bool IsPressedMouse(bool isRight) const;
	
	bool GetMouseDown(bool isRight) const;

	bool GetKeyDown(DirectX::Keyboard::Keys InKey) const;

	void GetMouseDelta(int32& OutX, int32& OutY) const;

	// 윈도우 상대적인 마우스 위치 값
	void GetMousePosition(int32& OutX, int32& OutY) const;

	void GetMousePositionNDC(int32& OutX, int32& OutY) const;

	int32 GetMouseWheel() const;

	int32 GetMouseWheelDelta() const;
	
private:
	uint32 WindowWidth;
	uint32 WindowHeight;
	
	std::unique_ptr<DirectX::Keyboard> Keyboard;
	std::unique_ptr<DirectX::Mouse> Mouse;
	std::unique_ptr<DirectX::GamePad> GamePad;
	
	// 이전의 입력 기록을 저장하여 상태 변화를 추적
	DirectX::Keyboard::KeyboardStateTracker KeyboardTracker;
	DirectX::Mouse::ButtonStateTracker MouseTracker;

	struct MouseState
	{
		bool LeftDown = false;
		bool RightDown = false;
		bool MiddleDown = false;
		int32 Wheel = 0; // Delta
		int32 X = 0;
		int32 Y = 0;
	};

	MouseState PrevMouseState;
	MouseState CurrentMouseState;
};
