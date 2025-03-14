#pragma once
#include "Debugging/DebugConsole.h"

template <typename T>
struct TCString
{
public:
	using CharType = T;

	// StrCmp method for comparing two strings
	static int StrCmp(const CharType* str1, const CharType* str2)
	{
		if constexpr (std::is_same_v<CharType, char>)
		{
			// Use std::strcmp for char strings
			return std::strcmp(str1, str2);
		}
		else if constexpr (std::is_same_v<CharType, wchar_t>)
		{
			// Use std::wcscmp for wchar_t strings
			return std::wcscmp(str1, str2);
		}
		else
		{
			static_assert(false, "Unsupported character type!");
			return 0;
		}
	}

	static CharType* Strcpy(CharType* dest, const CharType* src)
	{
		if constexpr (std::is_same_v<CharType, char>)
		{
			return strcpy_s(dest, src);
		}
		else if constexpr (std::is_same_v<CharType, wchar_t>)
		{
			return wcscpy_s(dest, src);
		}
		else
		{
			static_assert(false, "Unsupported character type!");
			return nullptr;
		}
	}

	static CharType* Strncpy(CharType* dest, const CharType* src, std::size_t count)
	{
		if constexpr (std::is_same_v<CharType, char>)
		{
			return strcpy_s(dest, src, count);
		}
		else if constexpr (std::is_same_v<CharType, wchar_t>)
		{
			return wcscpy_s(dest, src, count);
		}
		else
		{
			static_assert(false, "Unsupported character type!");
			return nullptr;
		}
	}

	static CharType* Strcat(CharType* dest, const CharType* src)
	{
		if constexpr (std::is_same_v<CharType, char>)
		{
			return strcat_s(dest, src);
		}
		else if constexpr (std::is_same_v<CharType, wchar_t>)
		{
			return wcscat_s(dest, src);
		}
		else
		{
			static_assert(false, "Unsupported character type!");
			return nullptr;
		}
	}

	static CharType* Strupr(CharType* str)
	{
		CharType* p = str;
		while (*p)
		{
			*p = std::toupper(static_cast<unsigned char>(*p));
			++p;
		}
		return str;
	}

	static int Strcmp(const CharType* str1, const CharType* str2)
	{
		if constexpr (std::is_same_v<CharType, char>)
		{
			return std::strcmp(str1, str2);
		}
		else if constexpr (std::is_same_v<CharType, wchar_t>)
		{
			return std::wcscmp(str1, str2);
		}
		else
		{
			static_assert(false, "Unsupported character type!");
			return 0;
		}
	}

	static int Strncmp(const CharType* str1, const CharType* str2, std::size_t count)
	{
		if constexpr (std::is_same_v<CharType, char>)
		{
			return std::strncmp(str1, str2, count);
		}
		else if constexpr (std::is_same_v<CharType, wchar_t>)
		{
			return std::wcsncmp(str1, str2, count);
		}
		else
		{
			static_assert(false, "Unsupported character type!");
			return 0;
		}
	}

	// 대소문자	구분 없이 문자열 비교
	static int Stricmp(const CharType* str1, const CharType* str2)
	{
		while (*str1 && (std::tolower(static_cast<unsigned char>(*str1)) == std::tolower(static_cast<unsigned char>(*str2))))
		{
			++str1;
			++str2;
		}
		return std::tolower(static_cast<unsigned char>(*str1)) - std::tolower(static_cast<unsigned char>(*str2));
	}

	static int Strnicmp(const CharType* str1, const CharType* str2, std::size_t count)
	{
		UE_LOG("Test for %d '%s' '%s'", count, str1, str2);
		while (count-- && *str1)
		{
			if (std::tolower(static_cast<unsigned char>(*str1)) != std::tolower(static_cast<unsigned char>(*str2)))
				return std::tolower(static_cast<unsigned char>(*str1)) - std::tolower(static_cast<unsigned char>(*str2));

			if (*str1 == '\0' || *str2 == '\0')
				break;

			++str1;
			++str2;
			UE_LOG("count: %d str1: '%s' str2: '%s'", count, str1, str2);
		}
		return 0;
	}
};
