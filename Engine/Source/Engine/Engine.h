#pragma once

#include "Core/Container/Map.h"
#include "Core/HAL/PlatformType.h"
#include "Core/AbstractClass/Singleton.h"
#include "Core/Rendering/UI.h"
#include "Core/Rendering/URenderer.h"
#include "EngineConfig.h"

class UObject;
class UWorld;

enum class EScreenMode : uint8
{
    Windowed,    // 창 모드
    Fullscreen,  // 전체화면 모드
    Borderless,  // 테두리 없는 창 모드
};

class UEngine : public TSingleton<UEngine>
{
public:
    // 각종 윈도우 관련 메시지(이벤트)를 처리하는 함수
    static LRESULT CALLBACK WndProc(HWND hWnd, uint32 uMsg, WPARAM wParam, LPARAM lParam);

    /**
     * Application을 초기화 합니다.
     * @param hInstance 창 인스턴스
     * @param InWindowTitle 윈도우 창 이름
     * @param InWindowClassName 윈도우 클래스 이름
     * @param InScreenWidth 화면 너비
     * @param InScreenHeight 화면 높이
     * @param InScreenMode 창 모드
     * @return 초기화 여부
     */
    void Initialize(
        HINSTANCE hInstance, const WCHAR* InWindowTitle, const WCHAR* InWindowClassName, int InScreenWidth,
        int InScreenHeight, EScreenMode InScreenMode = EScreenMode::Windowed
    );
    void Run();

    /**
     * Application에서 사용한 자원을 정리합니다.
     */
    void Shutdown();

	class URenderer* GetRenderer() const { return Renderer.get(); }
	float GetScreenRatio() const { return static_cast<float>(ScreenWidth) / ScreenHeight; }
    int GetScreenWidth() const { return ScreenWidth; }
    int GetScreenHeight() const { return ScreenHeight; }
    int GetInitializedScreenWidth() const { return InitializedScreenWidth; }
    int GetInitializedScreenHeight() const { return InitializedScreenHeight; }


private:
    void InitWindow(int InScreenWidth, int InScreenHeight);
    void InitRenderer();
    void InitWorld();
    void ShutdownWindow();
    void UpdateWindowSize(uint32 InScreenWidth, uint32 InScreenHeight);

public:
	UWorld* GetWorld() const { return World; }

    HWND GetWindowHandle() const { return WindowHandle; }

    template <typename ObjectType>
        requires std::derived_from<ObjectType, UObject>
    ObjectType* GetObjectByUUID(uint32 InUUID) const;
    UObject* GetObjectByUUID(uint32 InUUID) const;

    // World Grid
    float GetWorldGridGap() const { return WorldGridGap; }
    int32 GetWorldGridCellPerSide() const { return WorldGridCellPerSide; }

    void SetWorldGridGap(float InWorldGridGap) { WorldGridGap = InWorldGridGap; }

private:
    bool bIsExit = false;
    EScreenMode ScreenMode = EScreenMode::Windowed;

    const WCHAR* WindowTitle = nullptr;
    const WCHAR* WindowClassName = nullptr;
    HWND WindowHandle = nullptr;
    HINSTANCE WindowInstance = nullptr;

    int InitializedScreenWidth = 0;
    int InitializedScreenHeight = 0;

    int ScreenWidth = 0;
    int ScreenHeight = 0;

    int ResizeWidth = 0;
	int ResizeHeight = 0;

private:
	std::unique_ptr<URenderer> Renderer;

private:
	UI ui;

private:
    class UWorld* World;
    
    ////////
    // World Grid
    ////////
    AActor* WorldGrid = nullptr;

    float WorldGridGap = 1.f;

    /**
     * 초기 값 402에 대한 설명:
     *   정사각 모양의 월드 그리드는 크기가 제한되어있고, 카메라를 따라다니면서 WorldGripGap 값 만큼 스냅하며 xy 평면을 이동.
     *   만약 그리드의 크기가 충분히 크다면, 그리드의 끝은 카메라의 Far clip에 잘려서 보이지 않게 됨.
     *   하지만 그리드가 작다면, 그리드의 끝이 보임.
     *
     *   현재 카메라는 Far clip이 100으로 설정되어있으며, 카메라가 그리드의 중앙에 위치하므로,
     *   Far clip에 의해 그리드의 끝이 잘려나가려면 크기가 적어도 200이어야 함.
     *   UI에서 WorldGridGap 값은 최소 0.5로 제한되어있기 때문에 한 모서리에 위치한 그리드의 간격은 적어도 400이어야
     *   그리드의 크기가 200이 되며, 그리드의 끝이 카메라의 Far clip에 잘리게 됨. (200 / 0.5 = 400)
     *
     *   또한, 그리드는 스냅하며 움직이기 때문에 카메라의 위치에 따라 그리드의 가장 끝 부분이 카메라의 Far clip 내부에 존재할 수 있음.
     *   따라서 양쪽으로 길이 1씩 추가해서 402로 설정.
     */
    int32 WorldGridCellPerSide = 402;
    
public:
    // TArray<std::shared_ptr<UObject>> GObjects;
    TMap<uint32, std::shared_ptr<UObject>> GObjects;

private:
    FEngineConfig* EngineConfig;

public:
	FEngineConfig* GetEngineConfig() const { return EngineConfig; }

};

template <typename ObjectType> requires std::derived_from<ObjectType, UObject>
ObjectType* UEngine::GetObjectByUUID(uint32 InUUID) const
{
    if (const std::shared_ptr<UObject>* Obj = GObjects.Find(InUUID))
    {
        if (const auto PriComp = std::dynamic_pointer_cast<ObjectType, UObject>(*Obj))
        {
            return PriComp.get();
        }
    }
    return nullptr;
}
