#pragma once

#include "Core/HAL/ContainerAllocator.h"
#include "CString.h"

/*
 *	Unreal Engine의 Core/Public/Containers/UnrealString.h를 구현하는 것을 목표로 함
 */

/*
# TCHAR가 ANSICHAR인 경우
1. const ANSICHAR* 로 FString 생성
2. std::string에서 FString 생성

# TCHAR가 WIDECHAR인 경우
1. const ANSICHAR* 로 FString 생성
1. const WIDECHAR* 로 FString 생성
2. std::wstring에서 FString 생성
3. std::string에서 FString 생성
*/

enum : int8 { INDEX_NONE = -1 };

/** Determines case sensitivity options for string comparisons. */
namespace ESearchCase
{
	enum Type : uint8
	{
		/** Case-sensitive . Upper/lower casing must match for strings to be considered equal. */
		CaseSensitive,

		/** Ignore case. Upper/lower casing does not matter when making a comparison. */
		IgnoreCase,
	};
};

/** Determines search direction for string operations. */
namespace ESearchDir
{
	enum Type : uint8
	{
		/** Search from the start, moving forward through the string. */
		FromStart,

		/** Search from the end, moving backward through the string. */
		FromEnd,
	};
}

class FString
{
public:
	static const size_t npos = -1;

private:
	using BaseStringType = std::basic_string<
		TCHAR,
		std::char_traits<TCHAR>,
		FDefaultAllocator<TCHAR>
	>;

	BaseStringType PrivateString;

	friend struct std::hash<FString>;

public:
	FString() = default;
	~FString() = default;

	FString(const FString&) = default;
	FString& operator=(const FString&) = default;
	FString(FString&&) = default;
	FString& operator=(FString&&) = default;

	explicit FString(BaseStringType InString) : PrivateString(std::move(InString)) {}

#if IS_WIDECHAR
private:
	static std::wstring ConvertWideChar(const ANSICHAR* NarrowStr);

public:
	FString(const std::wstring& InString) : PrivateString(InString) {}
	FString(const std::string& InString) : PrivateString(ConvertWideChar(InString.c_str())) {}
	FString(const WIDECHAR* InString) : PrivateString(InString) {}
	FString(const ANSICHAR* InString) : PrivateString(ConvertWideChar(InString)) {}
#else
public:
	FString(const std::string& InString) : PrivateString(InString) {}
	FString(const ANSICHAR* InString) : PrivateString(InString) {}
#endif

	static FString FromInt(int32 Num);
	static FString SanitizeFloat(float InFloat);

public:
	FORCEINLINE int32 Len() const;
	FORCEINLINE bool IsEmpty() const;

	/** 배열의 모든 요소를 지웁니다. */
	void Empty();

	/**
	 * 문자열이 서로 같은지 비교합니다.
	 * @param Other 비교할 String
	 * @param SearchCase 대소문자 구분
	 * @return 같은지 여부
	 */
	bool Equals(const FString& Other, ESearchCase::Type SearchCase = ESearchCase::CaseSensitive) const;

	/**
	 * 문자열이 겹치는지 확인합니다.
	 * @param SubStr 찾을 문자열
	 * @param SearchCase 대소문자 구분
	 * @param SearchDir 찾을 방향
	 * @return 문자열 겹침 여부
	 */
	bool Contains(
		const FString& SubStr, ESearchCase::Type SearchCase = ESearchCase::IgnoreCase,
		ESearchDir::Type SearchDir = ESearchDir::FromStart
	) const;

	/**
	 * 문자열을 찾아 Index를 반홥합니다.
	 * @param SubStr 찾을 문자열
	 * @param SearchCase 대소문자 구분
	 * @param SearchDir 찾을 방향
	 * @param StartPosition 시작 위치
	 * @return 찾은 문자열의 Index를 반환합니다. 찾지 못하면 -1
	 */
	int32 Find(
		const FString& SubStr, ESearchCase::Type SearchCase = ESearchCase::IgnoreCase,
		ESearchDir::Type SearchDir = ESearchDir::FromStart, int32 StartPosition = -1
	) const;

	/**
	 * 대소문자 구분 없이 문자열을 지정된 길이만큼 비교합니다.
	 * @param Other 비교할 String
	 * @param Count 비교할 길이
	 * @return 비교 결과
	 */
	int Strnicmp(const FString& Other, const size_t Count) const;

	/**
	 * 문자열의 n번째 이후의 부분 문자열을 반환합니다.
	 * @param Pos 시작 위치
	 * @param Count 길이 (기본값: npos)
	 * @return 부분 문자열
	 */
	FString Substr(const size_t Pos, const size_t Count = npos) const;

	void RemoveAt(int32 Index, int32 Count, bool bAllowShrinking);

	/** 문자열의 첫 번째 문자를 반환합니다. */
	FORCEINLINE TCHAR& Front();
	FORCEINLINE const TCHAR& Front() const;

	/** 문자열의 마지막 문자를 반환합니다. */
	FORCEINLINE TCHAR& Back();
	FORCEINLINE const TCHAR& Back() const;

	/** 문자열의 마지막 문자를 제거합니다. */
	void PopBack();

	/** 문자열의 지정된 위치에서 지정된 길이만큼 제거합니다. */
	void RemoveAt(const size_t Pos, const size_t Count = npos);
	void RemoveAt(BaseStringType::iterator It, const size_t Count = npos);

	/** 문자열의 시작 반복자를 반환합니다. */
	FORCEINLINE BaseStringType::iterator Begin();
	FORCEINLINE BaseStringType::const_iterator Begin() const;

	/** 문자열을 대문자로 변환합니다. */
	FString ToUpper() const;

	/**
	 * Removes whitespace characters from the start and end of this string.
	 */
	void TrimStartAndEndInline();
	/**
	 * @note Unlike Trim() this function returns a copy, and does not mutate the string.
	 */
	FString TrimStartAndEnd() const&;
	FString TrimStartAndEnd()&&;

	/**
	 * Removes whitespace characters from the start of this string.
	 */
	void TrimStartInline();
	/**
	 * @note Unlike Trim() this function returns a copy, and does not mutate the string.
	 */
	FString TrimStart() const&;
	FString TrimStart()&&;

	/**
	 * Removes whitespace characters from the end of this string.
	 */
	void TrimEndInline();
	/**
	 * @note Unlike TrimTrailing() this function returns a copy, and does not mutate the string.
	 */
	FString TrimEnd() const&;
	FString TrimEnd()&&;

public:
	/** TCHAR* 로 반환하는 연산자 */
	FORCEINLINE const TCHAR* operator*() const;

	FORCEINLINE FString operator+(const FString& SubStr) const;
	FORCEINLINE FString& operator+=(const FString& SubStr);
	FORCEINLINE friend FString operator+(const FString& Lhs, const FString& Rhs);

	FORCEINLINE bool operator==(const FString& Rhs) const;
	FORCEINLINE bool operator==(const TCHAR* Rhs) const;

	/** n번째 위치의 문자를 가져오는 연산자 오버로딩 */
	FORCEINLINE TCHAR& operator[](const size_t Index);
	FORCEINLINE const TCHAR& operator[](const size_t Index) const;
};


FORCEINLINE int32 FString::Len() const
{
	return static_cast<int32>(PrivateString.length());
}

FORCEINLINE bool FString::IsEmpty() const
{
	return PrivateString.empty();
}

FORCEINLINE const TCHAR* FString::operator*() const
{
	return PrivateString.c_str();
}

FORCEINLINE FString FString::operator+(const FString& SubStr) const
{
	return static_cast<FString>(this->PrivateString + SubStr.PrivateString);
}

FString operator+(const FString& Lhs, const FString& Rhs)
{
	FString CopyLhs{ Lhs };
	return CopyLhs += Rhs;
}

FORCEINLINE bool FString::operator==(const FString& Rhs) const
{
	return Equals(Rhs, ESearchCase::IgnoreCase);
}

FORCEINLINE bool FString::operator==(const TCHAR* Rhs) const
{
	return Equals(Rhs);
}

FORCEINLINE FString& FString::operator+=(const FString& SubStr)
{
	this->PrivateString += SubStr.PrivateString;
	return *this;
}

FORCEINLINE TCHAR& FString::operator[](const size_t Index)
{
	return PrivateString[Index];
}

FORCEINLINE const TCHAR& FString::operator[](const size_t Index) const
{
	return PrivateString[Index];
}

FORCEINLINE TCHAR& FString::Front()
{
	return PrivateString.front();
}

FORCEINLINE const TCHAR& FString::Front() const
{
	return PrivateString.front();
}

FORCEINLINE TCHAR& FString::Back()
{
	return PrivateString.back();
}

FORCEINLINE const TCHAR& FString::Back() const
{
	return PrivateString.back();
}

FORCEINLINE void FString::PopBack()
{
	PrivateString.pop_back();
}

FORCEINLINE FString::BaseStringType::iterator FString::Begin()
{
	return PrivateString.begin();
}

FORCEINLINE FString::BaseStringType::const_iterator FString::Begin() const
{
	return PrivateString.begin();
}

template <>
struct std::hash<FString> {
	size_t operator()(const FString& InString) const {
		// 해시 계산 로직
		return std::hash<FString::BaseStringType>()(InString.PrivateString);
	}
};
