#pragma once

#include "pch.h"
#include "HAL/PlatformType.h"
#include "ISwitchable.h"
#include "ImGui/imgui_impl_dx11.h"
#include "EditorWindow.h"
#include "Container/Map.h"
#include "Container/Pair.h"
#include "Container/String.h"

class UEditorDesigner
{
public:
	static UEditorDesigner& Get()
	{
		static UEditorDesigner Instance;
		return Instance;
	}

	void AddWindow(const FString& WindowId, const std::shared_ptr<UEditorWindow>& Window)
	{
		Windows[WindowId] = Window;
	}
	
	/**
		캐스팅을 통해 Window를 가져옵니다.
		이후 setter 를 통해 값을 변경할 수 있습니다.
		auto window = UEditorDesigner::Get().GetWindow("mainWindow");
		if (window) {
			// dynamic_cast를 통해 MyWindow 타입으로 변환 후 setter 호출
			if (MyWindow* mw = dynamic_cast<MyWindow*>(window.get())) {
				mw->SetValue(42);
			}
		}
	*/
	// 현재 관리되고 있는 윈도우 창을 가져옵니다.
	std::shared_ptr<UEditorWindow> GetWindow(const FString& WindowId)
	{
		return *Windows.Find(WindowId);
		//auto it = Windows.find(WindowId);
		//if (it != Windows.end())
		//{
		//	return it->second;
		//}
		//return nullptr;
	}

	void OnResize(UINT32 Width, UINT32 Height)
	{
		if (Windows.IsEmpty()) return;

		for (const TPair<const FString, std::shared_ptr<UEditorWindow>>& Window : Windows)
		{
			Window.Value->OnResize(Width, Height);
		}
	}

	void Render()
	{
		if (Windows.IsEmpty()) return;

		for (const TPair<const FString, std::shared_ptr<UEditorWindow>>& Window : Windows)
		{
			Window.Value->Render();
		}
	}

	void Toggle()
	{
		if (Windows.IsEmpty()) return;
		for (const TPair<const FString, std::shared_ptr<UEditorWindow>>& Window : Windows)
		{
			if (auto Switchable = dynamic_cast<ISwitchable*>(Window.Value.get()))
			{
				Switchable->Toggle();
			}
		}
	}

	bool IsClear() const
	{
		return bFinishClearWindows;
	}

	void Clear()
	{
		bFinishClearWindows = false;
	}

private:
	UEditorDesigner() = default;
	~UEditorDesigner() = default;
	UEditorDesigner(const UEditorDesigner&) = delete;
	UEditorDesigner& operator=(const UEditorDesigner&) = delete;

	TMap<FString, std::shared_ptr<UEditorWindow>> Windows;

	bool bFinishClearWindows;
};