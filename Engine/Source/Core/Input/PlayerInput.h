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
	bool IsKeyPressed(DirectX::Keyboard::Keys InKey) const;

	bool IsKeyReleased(DirectX::Keyboard::Keys InKey) const;

	bool IsKeyDown(DirectX::Keyboard::Keys InKey) const;

	bool IsMousePressed(bool isRight) const;

	bool IsMouseReleased(bool isRight) const;
	
	bool IsMouseDown(bool isRight) const;

	void GetMouseDelta(int32& OutX, int32& OutY) const;

	// 윈도우 상대적인 마우스 위치 값
	void GetMousePosition(int32& OutX, int32& OutY) const;

	void GetMousePositionNDC(float& OutX, float& OutY) const;

	int32 GetMouseWheel() const;

	int32 GetMouseWheelDelta() const;

	// 커서의 현재 스크린 포지션을 캐싱
	void CacheCursorPosition();

	/**
	 * 캐싱된 스크린 포지션을 기반으로 위치 고정.
	 * SetCursorPos를 내부에서 사용하며, 이 함수는 스크린 포지션을 받음.
	 */
	void FixMouseCursor();
	
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
		
		int32 Wheel = 0; // 누적되는 값
		
		int32 X = 0;
		int32 Y = 0;

		/**
		 * 윈도우 메시지를 통한 자동 업데이트 대신 원하는 시점에 위치 정보를 업데이트하기 위해 별도로 저장.
		 * 이 값을 이용해서 시야 회전시 커서를 고정시켜 자유로운 시점 회전이 가능하게 됨.
		 * 윈도우 메시지는 커서를 고정시킴으로 인해 발생하는 커서의 위치 이동 또한 전달해주므로,
		 * 커서를 고정하면 시점 또한 고정되는 문제 발생.
		 * 따라서, 원하는 시점에만 위치를 업데이트할 수 있는 GetCursorPos를 별도로 사용 및
		 * 해당 값을 통해 스크린 위치에서의 커서 위치를 고정할 수 있음.
		 */
		int32 ScreenX = 0; 
		int32 ScreenY = 0;
	};

	MouseState PrevMouseState;
	MouseState CurrentMouseState;

	int32 CachedMouseX = 0;
	int32 CachedMouseY = 0;
};
