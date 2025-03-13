#include "pch.h" 
#include "String.h"
#include "Core/Math/MathUtility.h"

#if IS_WIDECHAR
std::wstring FString::ConvertWideChar(const ANSICHAR* NarrowStr)
{
    const int Size = MultiByteToWideChar(CP_UTF8, 0, NarrowStr, -1, nullptr, 0);
    std::wstring Str(Size, 0);
    MultiByteToWideChar(CP_UTF8, 0, NarrowStr, -1, Str.data(), Size);
    return Str;
}
#endif

FString FString::FromInt(int32 Num)
{
#if IS_WIDECHAR
    return FString{std::to_wstring(Num)};
#else
    return FString{std::to_string(Num)};
#endif
}

FString FString::SanitizeFloat(float InFloat)
{
#if IS_WIDECHAR
    return FString{std::to_wstring(InFloat)};
#else
    return FString{std::to_string(InFloat)};
#endif
}

void FString::Empty()
{
    PrivateString.clear();
}

bool FString::Equals(const FString& Other, ESearchCase::Type SearchCase) const
{
    const int32 Num = Len();
    const int32 OtherNum = Other.Len();

    if (Num != OtherNum)
    {
        // Handle special case where FString() == FString("")
        return Num + OtherNum == 1;
    }
    else if (Num > 1)
    {
        if (SearchCase == ESearchCase::CaseSensitive)
        {
            return TCString<TCHAR>::Strcmp(**this, *Other) == 0; 
        }
        else
        {
            return TCString<TCHAR>::Stricmp(**this, *Other) == 0;
        }
    }

    return true;
}

bool FString::Contains(const FString& SubStr, ESearchCase::Type SearchCase, ESearchDir::Type SearchDir) const
{
    return Find(SubStr, SearchCase, SearchDir, 0) != INDEX_NONE;
}

int32 FString::Find(
    const FString& SubStr, ESearchCase::Type SearchCase, ESearchDir::Type SearchDir, int32 StartPosition
) const
{
    if (SubStr.IsEmpty() || IsEmpty())
    {
        return INDEX_NONE;
    }

    const TCHAR* StrPtr = **this;
    const TCHAR* SubStrPtr = *SubStr;
    const int32 StrLen = Len();
    const int32 SubStrLen = SubStr.Len();

    auto CompareFunc = [SearchCase](TCHAR A, TCHAR B) -> bool {
        return (SearchCase == ESearchCase::IgnoreCase) ? 
            tolower(A) == tolower(B) : A == B;
    };

    auto FindSubString = [&](int32 Start, int32 End, int32 Step) -> int32 {
        for (int32 i = Start; i != End; i += Step)
        {
            bool Found = true;
            for (int32 j = 0; j < SubStrLen; ++j)
            {
                if (!CompareFunc(StrPtr[i + j], SubStrPtr[j]))
                {
                    Found = false;
                    break;
                }
            }
            if (Found)
            {
                return i;
            }
        }
        return INDEX_NONE;
    };

    if (SearchDir == ESearchDir::FromStart)
    {
        StartPosition = FMath::Clamp(StartPosition, 0, StrLen - SubStrLen);
        return FindSubString(StartPosition, StrLen - SubStrLen + 1, 1);
    }
    else // ESearchDir::FromEnd
    {
        StartPosition = (StartPosition == INDEX_NONE) ? StrLen - SubStrLen : FMath::Min(StartPosition, StrLen - SubStrLen);
        return FindSubString(StartPosition, -1, -1);
    }
}
