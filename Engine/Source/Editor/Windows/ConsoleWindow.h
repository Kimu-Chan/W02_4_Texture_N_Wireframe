#pragma once

//@TODO: UE_LOG()를 사용할 수 있도록 수정
#define UE_LOG(format, ...) if (UI::ConsoleWindowInstance) { UI::ConsoleWindowInstance->Log(format, ##__VA_ARGS__); }

#include "Editor/EditorWindow.h"
#include "Editor/ICommand.h"
#include "Editor/ISwitchable.h"
#include "ImGui/imgui.h"

class URenderer;
class FString;

class ConsoleWindow : public UEditorWindow, public ISwitchable, public ICommand
{
public:
	ConsoleWindow();
    ~ConsoleWindow() override;

	void Render() override;
    void OnResize(uint32 Width, uint32 Height) override;

    void Toggle() override;
    void Execute(const char*) override;

    /*static int   Stricmp(const char* s1, const char* s2) { int d; while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; } return d; }
    static int   Strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
    static int   Strncmp(const char* s1, const char* s2, int n) { return n ? (*s1 == *s2 ? (*s1 ? Strncmp(s1 + 1, s2 + 1, n - 1) : 0) : ((unsigned char)*s1 - (unsigned char)*s2)) : 0; }
    static char* Strdup(const char* s) { IM_ASSERT(s); size_t len = strlen(s) + 1; void* buf = ImGui::MemAlloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
    static void  Strtrim(char* s) { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }*/

    static int TextEditCallbackStub(ImGuiInputTextCallbackData* data)
    {
        ConsoleWindow* Console = static_cast<ConsoleWindow*>(data->UserData);
        return Console->TextEditCallback(data);
    }

    int TextEditCallback(ImGuiInputTextCallbackData* Data);

    void SetRenderer(URenderer* InRenderer) { MainRenderer = InRenderer; }

    static ImVec2 ResizeToScreen(const ImVec2& vec2, ImVec2 PreRatio, ImVec2 CurRatio);

	void Log(const char* format, ...);
private:

    void ClearLog();

    void ProcessCommand(const FString& cmd);

private:
    char                        InputBuf[256];
	std::vector<FString>        Items; // 출력 로그   //TODO: TArray로 변경???
	std::vector<FString>        Commands;   // TODO: const FString 적용 가능 여부 확인
	std::vector<FString>        History;
    int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter       Filter;
    bool                  AutoScroll;
    bool                  ScrollToBottom;

    bool bWasOpen;

    URenderer* MainRenderer;
};

