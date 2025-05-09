#pragma once

/*
 * Unreal Engine의 HAL/Platform.h를 목표로 하는 헤더
 * 그 중, PlatformType 구조체를 정의
 */

//~ Windows.h

#define WIN32_LEAN_AND_MEAN

#define _TCHAR_DEFINED  // TCHAR 재정의 에러 때문

#ifdef TEXT             // Windows.h의 TEXT를 삭제
	#undef TEXT
#endif
//~ Windows.h

// inline을 강제하는 매크로
#define FORCEINLINE __forceinline

// inline을 하지않는 매크로
#define FORCENOINLINE __declspec(noinline)

#define IS_WIDECHAR 1

#if IS_WIDECHAR 
	#define TEXT(x) L##x
#else
	#define TEXT(x) x
#endif

/*
//---------------------------------------------------------------------
// Utility for automatically setting up the pointer-sized integer type
//---------------------------------------------------------------------

template<typename T32BITS, typename T64BITS, int PointerSize>
struct SelectIntPointerType
{
	// nothing here are is it an error if the partial specializations fail
};

// sizeof(void*)를 통해 32비트와 64비트를 구분 가능
template<typename T32BITS, typename T64BITS>
struct SelectIntPointerType<T32BITS, T64BITS, 8>
{
	// Select the 64 bit type.
	typedef T64BITS TIntPointer;
};

template<typename T32BITS, typename T64BITS>
struct SelectIntPointerType<T32BITS, T64BITS, 4>
{
	// Select the 32 bit type.
	typedef T32BITS TIntPointer;
};
*/

/**
* Generic types for almost all compilers and platforms
**/

//~ Unsigned base types

// 8-bit unsigned integer
typedef unsigned char 		uint8;

// 16-bit unsigned integer
typedef unsigned short int	uint16;

// 32-bit unsigned integer
typedef unsigned int		uint32;

// 64-bit unsigned integer
typedef unsigned long long	uint64;

//~ Signed base types.

// 8-bit signed integer
typedef	signed char			int8;

// 16-bit signed integer
typedef signed short int	int16;

// 32-bit signed integer
typedef signed int	 		int32;

// 64-bit signed integer
typedef signed long long	int64;

//~ Character types

// An ANSI character. 8-bit fixed-width representation of 7-bit characters.
typedef char				ANSICHAR;

// A wide character. In-memory only. ?-bit fixed-width representation of the platform's natural wide character set. Could be different sizes on different platforms.
typedef wchar_t				WIDECHAR;

// An 8-bit character type. In-memory only. 8-bit representation. Should really be char8_t but making this the generic option is easier for compilers which don't fully support C++20 yet.
enum UTF8CHAR : unsigned char {};

// An 8-bit character type. In-memory only. 8-bit representation.
/* UE_DEPRECATED(5.0) */ [[deprecated("FPlatformTypes::CHAR8 is deprecated, please use FPlatformTypes::UTF8CHAR instead.")]]
typedef uint8				CHAR8;

// A 16-bit character type. In-memory only.  16-bit representation. Should really be char16_t but making this the generic option is easier for compilers which don't fully support C++11 yet (i.e. MSVC).
typedef uint16				CHAR16;

// A 32-bit character type. In-memory only. 32-bit representation. Should really be char32_t but making this the generic option is easier for compilers which don't fully support C++11 yet (i.e. MSVC).
typedef uint32				CHAR32;

// A switchable character. In-memory only. Either ANSICHAR or WIDECHAR, depending on a licensee's requirements.
#if IS_WIDECHAR
typedef WIDECHAR TCHAR;
#else
typedef ANSICHAR TCHAR;
#endif


//// Unsigned int. The same size as a pointer.
//typedef SelectIntPointerType<uint32, uint64, sizeof(void*)>::TIntPointer UPTRINT;
//
//// Signed int. The same size as a pointer.
//typedef SelectIntPointerType<int32, int64, sizeof(void*)>::TIntPointer PTRINT;
//
//// Unsigned int. The same size as a pointer.
//typedef UPTRINT SIZE_T;
//
//// Signed int. The same size as a pointer.
//typedef PTRINT SSIZE_T;

typedef int32					TYPE_OF_NULL;
typedef decltype(nullptr)		TYPE_OF_NULLPTR;
