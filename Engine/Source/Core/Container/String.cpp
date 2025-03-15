#include "pch.h" 
#include "String.h"
#include "Core/Math/MathUtility.h"
#include "Template/Template.h"

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
    return FString{ std::to_wstring(Num) };
#else
    return FString{ std::to_string(Num) };
#endif
}

FString FString::SanitizeFloat(float InFloat)
{
#if IS_WIDECHAR
    return FString{ std::to_wstring(InFloat) };
#else
    return FString{ std::to_string(InFloat) };
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

int FString::Strnicmp(const FString& Other, const size_t Count) const
{
    return TCString<TCHAR>::Strnicmp(**this, *Other, Count);
}

FString FString::Substr(const size_t Pos, const size_t Count) const
{
    if (Pos > PrivateString.length())
    {
        return FString();
    }
    return FString(PrivateString.substr(Pos, Count));
}

void FString::RemoveAt(const size_t Pos, const size_t Count)
{
    PrivateString.erase(Pos, Count);
}

void FString::RemoveAt(BaseStringType::iterator It, const size_t Count)
{
    PrivateString.erase(It, It + Count);
}

FString FString::ToUpper() const
{
    FString UpperString = *this;
    TCString<TCHAR>::Strupr(UpperString.PrivateString.data());
    return UpperString;
}

void FString::TrimStartAndEndInline()
{
    TrimEndInline();
    TrimStartInline();
}

FString FString::TrimStartAndEnd() const&
{
    FString Result(*this);
    Result.TrimStartAndEndInline();
    return Result;
}

FString FString::TrimStartAndEnd()&&
{
    TrimStartAndEndInline();
    return MoveTemp(*this);
}

void FString::TrimStartInline()
{
    int32 Pos = 0;
    while (Pos < Len() && std::isspace((*this)[Pos]))
    {
        Pos++;
    }
    RemoveAt(0, Pos);
}

FString FString::TrimStart() const&
{
    FString Result(*this);
    Result.TrimStartInline();
    return Result;
}

FString FString::TrimStart()&&
{
    TrimStartInline();
    return MoveTemp(*this);
}

void FString::TrimEndInline()
{
    int32 End = Len();
    while (End > 0 && std::isspace(((*this)[End - 1])))
    {
        End--;
    }
    RemoveAt(End, Len() - End);
}

FString FString::TrimEnd() const&
{
    FString Result(*this);
    Result.TrimEndInline();
    return Result;
}

FString FString::TrimEnd()&&
{
    TrimEndInline();
    return MoveTemp(*this);
}