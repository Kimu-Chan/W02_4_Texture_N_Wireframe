#include "pch.h" 
#include "PlayerInput.h"

#include "Editor/Windows/ConsoleWindow.h"
#include "Rendering/UI.h"

APlayerInput::APlayerInput()
{
    Keyboard = std::make_unique<DirectX::Keyboard>();
    Mouse = std::make_unique<DirectX::Mouse>();
    GamePad = std::make_unique<DirectX::GamePad>();
}

void APlayerInput::SetWindowSize(uint32 InWidth, uint32 InHeight)
{
    WindowWidth = InWidth;
    WindowHeight = InHeight;
}

void APlayerInput::UpdateInput()
{
    auto Kb = Keyboard->GetState();
    KeyboardTracker.Update(Kb);

    // 마우스 상태 가져오기
    auto MouseState = Mouse->GetState();
    MouseTracker.Update(MouseState);
    int X = MouseState.x; // 윈도우에 상대적
    int Y = MouseState.y;

    PrevMouseState = CurrentMouseState;
    
    CurrentMouseState.LeftDown = MouseState.leftButton;
    CurrentMouseState.RightDown = MouseState.rightButton;
    CurrentMouseState.MiddleDown = MouseState.middleButton;
    CurrentMouseState.Wheel = MouseState.scrollWheelValue;
    CurrentMouseState.X = X;
    CurrentMouseState.Y = Y;
}

bool APlayerInput::IsPressedKey(DirectX::Keyboard::Keys InKey) const
{
    return KeyboardTracker.pressed.IsKeyDown(InKey);
}

bool APlayerInput::IsPressedMouse(bool isRight) const
{
    if (isRight)
    {
        return !PrevMouseState.RightDown && CurrentMouseState.RightDown;
    }
    return !PrevMouseState.LeftDown && CurrentMouseState.LeftDown;
}

bool APlayerInput::GetMouseDown(bool isRight) const
{
    if (isRight)
    {
        return CurrentMouseState.RightDown;
    }
    return CurrentMouseState.LeftDown;
}

bool APlayerInput::GetKeyDown(DirectX::Keyboard::Keys InKey) const
{
    return Keyboard->GetState().IsKeyDown(InKey);
}

void APlayerInput::GetMouseDelta(int32& OutX, int32& OutY) const
{
    OutX = CurrentMouseState.X - PrevMouseState.X;
    OutY = CurrentMouseState.Y - PrevMouseState.Y;
}

void APlayerInput::GetMousePosition(int32& OutX, int32& OutY) const
{
    OutX = CurrentMouseState.X;
    OutY = CurrentMouseState.Y;
}

void APlayerInput::GetMousePositionNDC(float& OutX, float& OutY) const
{
    float HalfWidth = static_cast<float>(WindowWidth) / 2.f;
    float HalfHeight = static_cast<float>(WindowHeight) / 2.f;
    OutX = (static_cast<float>(CurrentMouseState.X) - HalfWidth) / HalfWidth;
    OutY = (static_cast<float>(CurrentMouseState.Y) - HalfHeight) / HalfHeight * -1.f;
}

int32 APlayerInput::GetMouseWheel() const
{
    return CurrentMouseState.Wheel;
}

int32 APlayerInput::GetMouseWheelDelta() const
{
    return CurrentMouseState.Wheel - PrevMouseState.Wheel;
}
