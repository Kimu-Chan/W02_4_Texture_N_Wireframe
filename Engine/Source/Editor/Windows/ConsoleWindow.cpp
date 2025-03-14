#include "pch.h"
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "ConsoleWindow.h"
#include "Container/String.h"
#include "HAL/PlatformType.h"
#include "Editor/EditorDesigner.h"
#include "CoreUObject/ObjectFactory.h"
#include "Components/PrimitiveComponent.h"
#include "Template/Template.h"
#include "Debugging/DebugConsole.h"

ConsoleWindow::ConsoleWindow()
{
    memset(InputBuf, 0, sizeof(InputBuf));
    HistoryPos = -1;

    // "CLASSIFY" is here to provide the test case where "C"+[tab] completes to "CL" and display multiple matches.
    Commands.emplace_back("HELP: Shows the help message");
    Commands.emplace_back("HISTORY: Shows the log history");
    Commands.emplace_back("CLEAR: Clears the console");
    Commands.emplace_back("CLASSIFY");
    Commands.emplace_back("UE_LOG(): Unreal Engine Log");
    Commands.emplace_back("stat: Shows the stat");
    Commands.emplace_back("new: Initialize Scene");
    Commands.emplace_back("save: Save Scene");
    Commands.emplace_back("load: Load Scene");
    Commands.emplace_back("spawn: Spawn Primitive Object");

    AutoScroll = true;
    ScrollToBottom = false;
    UE_LOG("Welcome to Engine !");

    bWasOpen = false;
}

ConsoleWindow::~ConsoleWindow()
{
    ClearLog();
	History.clear();
    //for (int i = 0; i < History.size(); i++)
    //{
    //    ImGui::MemFree(History[i]);
    //}
}

void ConsoleWindow::Render()
{
    if (!bWasOpen)
    {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();

	// HD1080 기준으로 화면 비율을 계산 //
    float scaleX = io.DisplaySize.x / 1920.0f;
    float scaleY = io.DisplaySize.y / 1080.0f;

    ImVec2 WinSize(io.DisplaySize.x * 0.8f, io.DisplaySize.y * 0.3f);

    ImGui::SetNextWindowPos(ImVec2(0, io.DisplaySize.y - WinSize.y - 5), ImGuiCond_Once);
    ImGui::SetNextWindowSize(WinSize, ImGuiCond_Once);

    ImGuiWindowFlags WindowFlags = 
        ImGuiWindowFlags_NoMove | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoCollapse;

    if (!ImGui::Begin("Console Command", &bWasOpen, WindowFlags))
    {
        ImGui::End(); // 반드시 호출해야 함
        return;
    }


    ImGui::TextWrapped(
        "This example implements a console with basic coloring, completion (TAB key) and history (Up/Down keys). A more elaborate "
        "implementation may want to store entries along with extra data such as timestamp, emitter, etc.");
    ImGui::TextWrapped("Enter 'HELP' for help.");

    // TODO: display items starting from the bottom

    if (ImGui::SmallButton("Add Debug Text")) { Log("%d some text", Items.size()); Log("some more text"); Log("display very important message here!"); }
    ImGui::SameLine();
    if (ImGui::SmallButton("Add Debug Error")) { Log("[error] something went wrong"); }
    ImGui::SameLine();
    if (ImGui::SmallButton("Clear")) { ClearLog(); }
    ImGui::SameLine();
    bool copy_to_clipboard = ImGui::SmallButton("Copy");
    //static float t = 0.0f; if (ImGui::GetTime() - t > 0.02f) { t = ImGui::GetTime(); AddLog("Spam %f", t); }

    ImGui::Separator();

    // Options menu //
    if (ImGui::BeginPopup("Options"))
    {
        ImGui::Checkbox("Auto-scroll", &AutoScroll);
        ImGui::EndPopup();
    }

    // Options, Filter //
    ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_O, ImGuiInputFlags_Tooltip);
    if (ImGui::Button("Options"))
        ImGui::OpenPopup("Options");
    ImGui::SameLine();
    Filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);

    ImGui::Separator();

    // Reserve enough left-over height for 1 separator + 1 input text
    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_HorizontalScrollbar))
    {
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::Selectable("Clear")) ClearLog();
            ImGui::EndPopup();
        }

        // 각 line을 별도의 entry로 표시하여, 색상을 변경하거나 사용자 정의 위젯을 추가 가능
        // raw text만 원한다면 ImGui::TextUnformatted(log.begin(), log.end())를 사용 가능
        // 수천 개의 항목이 있는 경우 이 접근 방식은 너무 비효율적일 수 있음
        // user-side clipping을 사용하여 보이는 항목만 처리해야 할 수 있음
        // clipper는 첫 번째 Item의 높이를 자동으로 측정한 다음, "보이는 영역"에만 항목을 표시하도록 "탐색"함
        // clipper를 사용하려면 standard loop를 다음과 같이 대체 가능
        //      for (int i = 0; i < Items.Size; i++)
        //   With:
        //      ImGuiListClipper clipper;
        //      clipper.Begin(Items.Size);
        //      while (clipper.Step())
        //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        // - Items가 균일하게 간격을 두고 배치되어 있어야 함 (same height).
        // - elements에 대한 인덱스를 통해 cheap random access가 가능해야 함 (이전 elements를 모두 처리하지 않고도 인덱스를 통해 접근할 수 있어야 함)
        // 필터가 활성화된 경우 이 코드를 그대로 사용할 수 없음. 이는 'cheap random access' property를 깨뜨리기 때문
        // 필터링 테스트를 통과한 항목의 인덱스 또는 오프셋 배열을 미리 계산하고, 사용자가 필터를 변경할 때 이 배열을 다시 계산하며,
        // 새로 추가된 항목을 추가하는 것이 일반적인 응용 프로그램에서 필요할 수 있습니다. 이 작업은 예제 코드가 개선될 때까지 사용자에게 남겨둡니다!
        // 항목의 높이가 가변적인 경우(variable height):
        // - 동일한 높이의 항목으로 분할하는 것이 더 간단하며, 목록에서 random-seeking을 용이하게 합니다.
        // - IsRectVisible()을 manual call하고 Items의 extraneous decoration을 건너뛰는 것을 고려할 것
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
        if (copy_to_clipboard)
            ImGui::LogToClipboard();
        for (const FString item : Items)
        {
            if (!Filter.PassFilter(*item))
                continue;

            // Normally you would store more information in your item than just a string.
            // (e.g. make Items[] an array of structure, store color/type etc.)
            ImVec4 color;
            bool has_color = false;
            if (item.Find("[error]")) { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
            else if (item.Strnicmp("# ", 2) == 0) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
            if (has_color)
                ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(*item);
            if (has_color)
                ImGui::PopStyleColor();
        }
        if (copy_to_clipboard)
            ImGui::LogFinish();

        // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
        // Using a scrollbar or mouse-wheel will take away from the bottom edge.
        if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
            ImGui::SetScrollHereY(1.0f);
        ScrollToBottom = false;

        ImGui::PopStyleVar();
    }
    ImGui::EndChild();

    ImGui::Separator();

    // Command Line //
    bool bReclaimFocus = false;
    ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
	if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), input_text_flags, &TextEditCallbackStub, (void*)this))
    {
        FString inputStr = InputBuf;
        if (!inputStr.IsEmpty())
        {
			// Add to Items(a.k.a Log) //
            Items.push_back("# " + inputStr);
            // Insert into History //
            History.push_back(inputStr);
            HistoryPos = static_cast<int>(History.size());
			// Excute Command //
            ProcessCommand(inputStr);
        }
        InputBuf[0] = '\0';
        bReclaimFocus = true;
    }

    // Focus Setting //
    // Auto-focus on window apparition
    ImGui::SetItemDefaultFocus();
    if (bReclaimFocus)
        ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

    ImGui::End();
}

void ConsoleWindow::OnResize(UINT32 Width, UINT32 Height)
{
	//ResizeToScreen();
}

void ConsoleWindow::Toggle()
{
    bWasOpen = !bWasOpen;
}

void ConsoleWindow::Execute(const char* command)
{
    ProcessCommand(command);
}

int ConsoleWindow::TextEditCallback(ImGuiInputTextCallbackData* Data)
{
    //AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
    switch (Data->EventFlag)
    {
    // 자동 완성 // //@TODO: 수정이 필요함, tab키 누르면 마지막 글자가 바뀜, 정상적으로 match 하지 못하고 있음
    case ImGuiInputTextFlags_CallbackCompletion:
    {
		// 현재 커서가 있는 단어의 시작을 찾음
        const char* WordEnd = Data->Buf + Data->CursorPos;
        const char* WordStart = WordEnd;
        while (WordStart > Data->Buf)
        {
	        if (const char c = WordStart[-1]; c == ' ' || c == '\t' || c == ',' || c == ';')
                break;
            WordStart--;
        }

        // Build a list of candidates
        ImVector<const char*> Candidates;
        for (const auto& Command : Commands)
	        if (Command.Strnicmp(WordStart, static_cast<int>(WordEnd - WordStart)) == 0)
                Candidates.push_back(*Command);

        if (Candidates.empty())
        {
            // No match
            UE_LOG("No match for \"%.*s\"!\n", static_cast<int>(WordEnd - WordStart), WordStart);
        }
        else if (Candidates.size() == 1)
        {
            // 후보가 하나인 경우. Delete 단어의 시작을 지우고 완전히 대체하여 대문자(대부분의 경우)로 통일시켜준다 (정의된 Command case를 따름)
            Data->DeleteChars(static_cast<int>(WordStart - Data->Buf), static_cast<int>(WordEnd - WordStart));
            Data->InsertChars(Data->CursorPos, Candidates[0]);
            Data->InsertChars(Data->CursorPos, " ");
        }
        else
        {
            // 후보가 여럿인 경우. 겹치는 글자 까지 완성
            // "C"를 입력하고 + Tab을 누를시, "CL"까지 입력되고 "CLEAR" 와 "CLASSIFY" 를 후보로 보여준다.
            int MatchLen = static_cast<int>(WordEnd - WordStart);
            for (;;)
            {
                int c = 0;
                bool all_candidates_matches = true;
                for (int i = 0; i < Candidates.size() && all_candidates_matches; i++)
                    if (i == 0)
                        c = std::toupper(Candidates[i][MatchLen]);
                    else if (c == 0 || c != Candidates[i][MatchLen])
                        all_candidates_matches = false;
                if (!all_candidates_matches)
                    break;
                MatchLen++;
            }

            if (MatchLen > 0)
            {
                Data->DeleteChars(static_cast<int>(WordStart - Data->Buf), static_cast<int>(WordEnd - WordStart));
                Data->InsertChars(Data->CursorPos, Candidates[0], Candidates[0] + MatchLen);
            }

            // List matches
            UE_LOG("Possible matches:\n");
            for (const auto& Candidate : Candidates)
                UE_LOG("- %s\n", Candidate);
        }
        break;
    }
    // 이전 명령어 불러오기 //
    case ImGuiInputTextFlags_CallbackHistory:
    {
        // Example of HISTORY
        const int PrevHistoryPos = HistoryPos;
        if (Data->EventKey == ImGuiKey_UpArrow)
        {
            if (HistoryPos == -1)
                HistoryPos = static_cast<int>(History.size()) - 1;
            else if (HistoryPos > 0)
                HistoryPos--;
        }
        else if (Data->EventKey == ImGuiKey_DownArrow)
        {
            if (HistoryPos != -1)
                if (++HistoryPos >= static_cast<int>(History.size()))
                    HistoryPos = -1;
        }

        // A better implementation would preserve the data on the current input line along with cursor position.
        if (PrevHistoryPos != HistoryPos)
        {
            const FString HistoryStr = (HistoryPos >= 0) ? History[HistoryPos] : "";
            Data->DeleteChars(0, Data->BufTextLen);
            Data->InsertChars(0, *HistoryStr);
        }
        break;
    }
    default: ;
    }
    return 0;
}

void ConsoleWindow::Log(const char* format, ...)
{
    // FIXME-OPT
    //char buf[1024];
    //va_list args;
    //va_start(args, format);
    //vsnprintf(buf, IM_ARRAYSIZE(buf), format, args);
    //buf[IM_ARRAYSIZE(buf) - 1] = 0;
    //va_end(args);

    //Items.emplace_back(buf);

    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    Items.emplace_back(buffer);

    Debug::LogItem(buffer);
}

void ConsoleWindow::ClearLog()
{
    //for (int i = 0; i < Items.Size; i++)
    //    ImGui::MemFree(Items[i]);
    Items.clear();
}

void ConsoleWindow::ProcessCommand(const FString& cmd)
{
    // Process command //
	//@note: Cast to FString for case-insensitive comparison
    if (cmd == static_cast<FString>("clear"))
    {
        ClearLog();
    }
    else if (cmd == static_cast<FString>("help") || cmd == static_cast<FString>("?"))
    {
		UE_LOG("Commands:");
        for (const auto& Command : Commands)
            UE_LOG("- %s", *Command);
    }
    else if (cmd == static_cast<FString>("history"))
    {
        int First = static_cast<int>(History.size()) - 10;
        for (int i = First > 0 ? First : 0; i < static_cast<int>(History.size()); i++)
            UE_LOG("%3d: %s", i, *History[i]);
    }
    else if (cmd == static_cast<FString>("stat"))
    {
        auto Window = UEditorDesigner::Get().GetWindow("StatWindow");
        if (Window)
        {
            // dynamic_cast를 통해 MyWindow 타입으로 변환 후 setter 호출
            if (auto Stat = dynamic_cast<ISwitchable*>(Window.get()))
            {
                Stat->Toggle();
            }
        }
    }
    else if (cmd.Strnicmp("spawn ", 6) == 0)
    {
        // "spawn " 이후 문자열 추출
		FString args = *cmd.Substr(6);

        if (!args.IsEmpty())
        {
            bool bNeedSpawn = false;
            // shape에 무엇을 넣어야하는지 저장
            int SpawnCount = 0;

            // 첫 번째 공백을 찾아 shape와 count 분리
            size_t pos = args.Find(" ");
            if (pos != FString::npos)
            {
                FString shape = args.Substr(0, pos);
                FString countStr = args.Substr(pos + 1);

                //@TODO: Shape 유효성 검사 분리
                // shape가 유효한지 확인
                if (shape == static_cast<FString>("cube") || shape == static_cast<FString>("sphere") || shape == static_cast<FString>("triangle"))
                {
                    if (int Count = std::stoi(*countStr); Count > 0)
                    {
                        // 실제 객체 생성을 위한 로직 호출 부분
                        UE_LOG("Spawning %d %s(s)...", Count, *shape);
						SpawnCount = Count;

                        for (int i = 0; i < Count; i++)
                        {
                            FVector pos(i * 2, 0, 0);
                            if (shape == "cube")
                            {
                                Log("Try to Spawn Cube!\n");
                                //UCubeComponent* c = UObjectFactory::GetInst().ConstructObject<UCubeComponent>(UCubeComponent::GetClass(), MainRenderer);
                                //c->SetRelativeLocation(pos);
                            }
                            else if (shape == "sphere")
                            {
                                Log("Try to Spawn Sphere!\n");
                                //USphereComponent* s = UObjectFactory::GetInst().ConstructObject<USphereComponent>(USphereComponent::GetClass(), MainRenderer);
                                //s->SetRelativeLocation(pos);
                            }
                            else if (shape == "triangle")
                            {
                                Log("Try to Spawn Triangle!\n");
                                //UTriangleComponent* t = UObjectFactory::GetInst().ConstructObject<UTriangleComponent>(UTriangleComponent::GetClass(), MainRenderer);
                                //t->SetRelativeLocation(pos);
                            }
                        }
                    }
                    else
                    {
						bNeedSpawn = false;
                        UE_LOG("잘못된 개수입니다: %s", *countStr);
                    }
                }
                else
                {
					bNeedSpawn = false;
                    UE_LOG("알 수 없는 객체 타입입니다: '%s'", *shape);
                }

                //@TODO: Spawn 개수 저장
                //if (int Count = std::stoi(*countStr); Count > 0)
                //{
                //    // 실제 객체 생성을 위한 로직 호출 부분
                //    UE_LOG("Spawning %d %s(s)...", Count, *shape);
                //    SpawnCount = Count;
                //}
                //else
                //{
                //    bNeedSpawn = false;
                //    UE_LOG("잘못된 개수입니다: %s", *countStr);
                //}
            }
            else
            {
                //@TODO: shape 유효성 검사 분리
                // args가 유효한 shape인지 확인
                if (args == "cube" || args == "sphere" || args == "triangle")
                {
                    // 실제 객체 생성을 위한 로직 호출 부분
                    UE_LOG("Spawning 1 %s(s)...\n", *args);
                    // 예: SpawnShape(shape, count);

                    FVector pos(0, 0, 0);
                    if (args == "cube")
                    {
                        Log("Try to Spawn Cube!\n");
                        //UCubeComponent* c = UObjectFactory::GetInst().ConstructObject<UCubeComponent>(UCubeComponent::GetClass(), MainRenderer);
                        //c->SetRelativeLocation(pos);
                    }
                    else if (args == "sphere")
                    {
                        Log("Try to Spawn Sphere!\n");
                        //USphereComponent* s = UObjectFactory::GetInst().ConstructObject<USphereComponent>(USphereComponent::GetClass(), MainRenderer);
                        //s->SetRelativeLocation(pos);
                    }
                    else if (args == "triangle")
                    {
                        Log("Try to Spawn Triangle!\n");
                        //UTriangleComponent* t = UObjectFactory::GetInst().ConstructObject<UTriangleComponent>(UTriangleComponent::GetClass(), MainRenderer);
                        //t->SetRelativeLocation(pos);
                    }
				}
                else
                {
                    bNeedSpawn = false;
                    UE_LOG("알 수 없는 객체 타입입니다: '%s'", *args);
                }
            }
            //@TODO: Spawn 로직 호출
        }
        else
        {
            UE_LOG("사용법: spawn <cube|sphere|triangle> <count = 1>");
        }
    }
    else if (cmd.Strnicmp("UE_LOG(", 7) == 0)
    {
        // UE_LOG( 이후 문자열 추출
        FString args = cmd.Substr(7);
        size_t endPos = args.Find(")", ESearchCase::IgnoreCase ,ESearchDir::FromEnd);
        if (endPos != FString::npos)
        {
            args = args.Substr(0, endPos);
        }

        size_t commaPos = args.Find(",");
        if (commaPos != FString::npos)
        {
            FString formatPart = args.Substr(0, commaPos); // 포맷 부분
            FString argPart = args.Substr(commaPos + 1);            // 인자 부분

            size_t firstQuote = formatPart.Find("\""); // 앞 따옴표
            size_t lastQuote = formatPart.Find("\"", ESearchCase::IgnoreCase, ESearchDir::FromEnd); // 뒷 따옴표

            if (firstQuote != FString::npos &&
                lastQuote != FString::npos &&
                firstQuote != lastQuote)
            {
                FString formatStr = formatPart.Substr(firstQuote + 1, lastQuote - firstQuote - 1);
                char buffer[1024];

                // %d, %f, %s 서식 지정자에 따른 변환 처리
                if (formatStr.Find("%d") != FString::npos)
                {
                    int argValue = atoi(*argPart);
                    snprintf(buffer, sizeof(buffer), *formatStr, argValue);
                }
                else if (formatStr.Find("%f") != FString::npos)
                {
                    double argValue = atof(*argPart);
                    snprintf(buffer, sizeof(buffer), *formatStr, argValue);
                }
                else if (formatStr.Find("%s") != FString::npos)
                {
                    // 인자가 따옴표로 감싸져 있다면 제거
                    if (!argPart.IsEmpty() && argPart.Front() == '"' && argPart.Back() == '"')
                    {
                        argPart = argPart.Substr(1, argPart.Len() - 2);
                    }
                    snprintf(buffer, sizeof(buffer), *formatStr, *argPart);
                }
                else
                {
                    // 서식 지정자가 없는 경우 기본적으로 인자를 정수로 처리
                    int argValue = atof(*argPart);
                    snprintf(buffer, sizeof(buffer), *formatStr, argValue);
                }

                Log("%s", buffer);
            }
            else
            {
                Log("FORMAT ERROR: '%s'\n", cmd);
            }
        }
        else
        {
            Log("ARGUMENT ERROR: '%s'\n", cmd);
        }
    }
    else
    {
        UE_LOG("Unknown command: '%s'", *cmd);
    }

    // On command input, we scroll to bottom even if AutoScroll==false
    ScrollToBottom = true;
}

ImVec2 ConsoleWindow::ResizeToScreen(const ImVec2& vec2, ImVec2 PreRatio, ImVec2 CurRatio)
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

    return { vec2.x * PreRatio.x / CurRatio.x * min / preMin, vec2.y * PreRatio.y / CurRatio.y * min / preMin };
}
