#pragma once

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

#define IS_WIDECHAR 0

#if IS_WIDECHAR 
    #define TEXT(x) L##x
#else
    #define TEXT(x) x
#endif

// unsigned int type
typedef std::uint8_t uint8;
typedef std::uint16_t uint16;
typedef std::uint32_t uint32;
typedef std::uint64_t uint64;

// signed int
typedef std::int8_t int8;
typedef std::int16_t int16;
typedef std::int32_t int32;
typedef std::int64_t int64;

typedef char ANSICHAR;
typedef wchar_t WIDECHAR;

#if IS_WIDECHAR
    typedef WIDECHAR TCHAR;
#else
    typedef ANSICHAR TCHAR;
#endif
