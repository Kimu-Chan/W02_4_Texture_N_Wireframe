#include "pch.h"
#include "Debugging/DebugConsole.h"
#include <cstdarg>
#include <algorithm>
#include "Core/Container/String.h"

#include "ImGui/imgui_internal.h"

std::vector<FString> Debug::items;


void Debug::ShowConsole(bool bWasWindowSizeUpdated, ImVec2 PreRatio, ImVec2 CurRatio)
{    
    static char inputBuf[256] = "";
    static std::vector<FString> history;
    static int historyPos = -1;
    bool reclaimFocus = false;

    ImGui::Begin("Console");
    if (bWasWindowSizeUpdated)
    {
        auto* Window = ImGui::GetCurrentWindow();
        ImGui::SetWindowPos(ResizeToScreen(Window->Pos, PreRatio, CurRatio));
        ImGui::SetWindowSize(ResizeToScreen(Window->Size, PreRatio, CurRatio));
    }

     if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true, ImGuiWindowFlags_HorizontalScrollbar))
     {
         auto* Window = ImGui::GetCurrentWindow();
         ImGui::SetWindowPos(ResizeToScreen(Window->Pos, PreRatio, CurRatio));
         ImGui::SetWindowSize(ResizeToScreen(Window->Size, PreRatio, CurRatio));
         
         for (const auto& Item : items)
             ImGui::TextUnformatted(*Item);
    
         if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
             ImGui::SetScrollHereY(1.0f);
     }
     ImGui::EndChild();

     if (ImGui::InputText("Input", inputBuf, IM_ARRAYSIZE(inputBuf),
         ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory,
         [](ImGuiInputTextCallbackData* data) -> int
         {
             if (data->EventFlag == ImGuiInputTextFlags_CallbackHistory)
             {
                 if (history.empty()) return 0;
                 historyPos += (data->EventKey == ImGuiKey_UpArrow) ? -1 : 1;
                 historyPos = std::clamp(historyPos, 0, (int)history.size() - 1);
    
                 FString& historyCommand = history[historyPos];
                 data->DeleteChars(0, data->BufTextLen);
                 data->InsertChars(0, *historyCommand);
             }
             return 0;
         }))
     {
         FString inputStr = inputBuf;
         if (!inputStr.IsEmpty())
         {
             items.push_back("> " + inputStr);
             history.push_back(inputStr);
             historyPos = (int)history.size();
             ProcessCommand(inputStr, items);
         }
         inputBuf[0] = '\0';
         reclaimFocus = true;
     }

     if (reclaimFocus)
         ImGui::SetKeyboardFocusHere(-1);
    ImGui::End();
}

void Debug::ProcessCommand(const FString& command, std::vector<FString>& log)
{
    log.push_back("Executing: " + command);

    if (command == "clear")
    {
        log.clear();
    }
    else if (command == "help")
    {
        log.push_back("Available commands:");
        log.push_back("- clear: Clears the console.");
        log.push_back("- help: Shows this help message.");
    }
    else
    {
        log.push_back("Unknown command: " + command);
    }
}

void Debug::Log(const char* format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    items.emplace_back(buffer);
}

ImVec2 Debug::ResizeToScreen(const ImVec2& vec2, ImVec2 PreRatio, ImVec2 CurRatio)
{
    float min;
    if (CurRatio.x < CurRatio.y)
    {
        min = CurRatio.x;
    }
    else
    {
        min = CurRatio.y;
    }

    float preMin;
    if (PreRatio.x < PreRatio.y)
    {
        preMin = PreRatio.x;
    }
    else
    {
        preMin = PreRatio.y;
    }
    
    return {vec2.x * PreRatio.x / CurRatio.x * min / preMin, vec2.y * PreRatio.y / CurRatio.y * min / preMin};
}
