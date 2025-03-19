#include "pch.h"
#include "Name.h"
#include "NameTypes.h"
#include "HAL/PlatformType.h"
#include "Container/Map.h"
#include "AbstractClass/Singleton.h"

struct FNameEntryIds
{
	FNameEntryId ComparisonId, DisplayId;
};

/*-----------------------------------------------------------------------------
    FName helpers.
-----------------------------------------------------------------------------*/

/**
 * ANSICAHR나 WIDECHAR를 담는 인터페이스
 */
struct FNameStringView
{
    FNameStringView() : Data(nullptr), Len(0), bIsWide(false) {}
    FNameStringView(const ANSICHAR* Str, uint32 InLen) : Ansi(Str), Len(InLen), bIsWide(false) {}
    FNameStringView(const WIDECHAR* Str, uint32 InLen) : Wide(Str), Len(InLen), bIsWide(true) {}
    FNameStringView(const void* InData, uint32 InLen, bool bInIsWide) : Data(InData), Len(InLen), bIsWide(bInIsWide) {}

    union
    {
        const void* Data;
        const ANSICHAR* Ansi;
        const WIDECHAR* Wide;
    };

    uint32 Len;
    bool bIsWide;

    bool IsAnsi() const { return !bIsWide; }
};

/*
struct FNameHash
{
	uint32 Index;
	FNameEntryHeader EntryProbeHeader; // 엔트리들을 조사할 때 동일성 확인에 도움을 줌

#pragma region Hash - djb2
	//TODO: int32 Len 파라미터 추가
	template <typename CharType>
	static uint32 GenerateHash(const CharType* Str)
	{
		// djb2 문자열 해싱 알고리즘
		uint32 Hash = 5381;
		while (*Str)
		{
			Hash = ((Hash << 5) + Hash) + *Str;
			++Str;
		}
		return Hash;
	}
#pragma endregion

	template<class CharType>
	FNameHash(const CharType* Str, int32 Len)
		: FNameHash(GenerateHash(Str), Len, sizeof(CharType) == sizeof(WIDECHAR))	//TODO: GenerateHash 함수에 Len 파라미터 추가
	{}

	FNameHash(uint64 Hash, int32 Len, bool bIsWide)
	{
		Index = Hash;
		EntryProbeHeader.Len = Len;
		EntryProbeHeader.bIsWide = bIsWide;
	}

};
*/

namespace // TODO: Use FNameHash, not uint32
{
	template <typename CharType>
	uint32 HashString(const CharType* Str)
	{
		// djb2 문자열 해싱 알고리즘
		uint32 Hash = 5381;
		while (*Str)
		{
			Hash = ((Hash << 5) + Hash) + *Str;
			++Str;
		}
		return Hash;
	}

	template <typename CharType>
	FORCENOINLINE uint32 HashStringLower(const CharType* Str, uint32 InLen)
	{
		CharType LowerStr[NAME_SIZE];
		if constexpr (std::is_same_v<CharType, wchar_t>)
		{
			for (uint32 i = 0; i < InLen; i++)
			{
				LowerStr[i] = towlower(Str[i]);
			}
			LowerStr[InLen] = '\0';
		}
		else
		{
			for (uint32 i = 0; i < InLen; ++i)
			{
				LowerStr[i] = static_cast<CharType>(tolower(Str[i]));
			}
			LowerStr[InLen] = '\0';
		}
		return HashString(LowerStr);
	}

	template <ENameCase Sensitivity>
	uint32 HashName(FNameStringView InName);

	template <>
	uint32 HashName<ENameCase::IgnoreCase>(FNameStringView InName)
	{
		return InName.IsAnsi() ? HashStringLower(InName.Ansi, InName.Len) : HashStringLower(InName.Wide, InName.Len);
	}

	template <>
	uint32 HashName<ENameCase::CaseSensitive>(FNameStringView InName)
	{
		return InName.IsAnsi() ? HashString(InName.Ansi) : HashString(InName.Wide);
	}
}

template <ENameCase Sensitivity>
struct FNameValue
{
	explicit FNameValue(FNameStringView InName)
		: Name(InName)
		, Hash(HashName<Sensitivity>(InName))
	{}

	FNameStringView Name;
	uint32 Hash;	// TODO: Use FNameHash, not uint32
#if WITH_CASE_PRESERVING_NAME
	FNameEntryId ComparisonId;
#endif
};

using FNameComparisonValue = FNameValue<ENameCase::IgnoreCase>;
#if WITH_CASE_PRESERVING_NAME
using FNameDisplayValue = FNameValue<ENameCase::CaseSensitive>;
#endif

/**
 * FNameEntry를 관리하는 Pool 클래스
 * @TODO: Shard 미구현!
 */
struct FNamePool : public TSingleton<FNamePool>
{
private:
#if WITH_CASE_PRESERVING_NAME
	TMap<uint32, FNameEntry> DisplayPool;	//TODO: uint32를 FNameEntryId로 변경
#endif
    TMap<uint32, FNameEntry> ComparisonPool;

public:
	//FNamePool();

	FNameEntryId	Store(FNameStringView& View);
	FNameEntryId	Find(FNameStringView View) const;

	/** Hash로 원본 문자열을 가져옵니다. */
	FNameEntry		Resolve(uint32 Hash) const { return *DisplayPool.Find(Hash); }	//TODO: FNameEntry&를 반환하는 FNameEntryId를 인자로 하도록 변경

	//bool			IsValid(FNameEntryHandle Handle) const;

	//FDisplayNameEntryId	StoreValue(const FNameComparisonValue& Value);

	uint32			NumEntries() const { return DisplayPool.Num() + ComparisonPool.Num(); };
private:
	template <ENameCase Sensitivity>
	FNameEntry MakeEntry(const FNameValue<Sensitivity>& Value) const
	{
		FNameEntry Result;
		Result.ComparisonId = Value.ComparisonId;
		Result.Header = {
			.bIsWide = Value.Name.bIsWide,
			.Len = static_cast<uint16>(Value.Name.Len)
		};
		if (Value.Name.bIsWide)
		{
			Result.StoreName(Value.Name.Wide, Value.Name.Len);
		}
		else
		{
			Result.StoreName(Value.Name.Ansi, Value.Name.Len);
		}
		return Result;
	}

public:
	/**
	 * 문자열을 찾거나, 없으면 Hash화 해서 저장합니다.
	 *
	 * @return DisplayName의 Hash
	 */
	FNameEntryId Store(const FNameStringView& Name)
	{
#if WITH_CASE_PRESERVING_NAME
		// DisplayPool에 같은 문자열이 있다면, 문자열의 Hash 반환
		FNameDisplayValue DisplayValue{ Name };
		if (DisplayPool.Find(DisplayValue.Hash))
		{
			return { DisplayValue.Hash };
		}
#endif

		const FNameComparisonValue ComparisonValue{ Name };
		if (!ComparisonPool.Find(ComparisonValue.Hash))
		{
			const FNameEntry Entry = MakeEntry(ComparisonValue);
			ComparisonPool.Add(ComparisonValue.Hash, Entry);
		}

		DisplayValue.ComparisonId = { ComparisonValue.Hash };
		DisplayPool.Add(DisplayValue.Hash, MakeEntry(DisplayValue));
		return { DisplayValue.Hash };
	}
};

/*-----------------------------------------------------------------------------
    FNameEntry
-----------------------------------------------------------------------------*/

void FNameEntry::StoreName(const ANSICHAR* InName, uint32 Len)
{
    FPlatformMemory::Memcpy(AnsiName, InName, sizeof(ANSICHAR) * Len);
    AnsiName[Len] = '\0';
}

void FNameEntry::StoreName(const WIDECHAR* InName, uint32 Len)
{
    FPlatformMemory::Memcpy(WideName, InName, sizeof(WIDECHAR) * Len);
    WideName[Len] = '\0';
}

/*
void FNameEntry::CopyUnterminatedName(ANSICHAR* Out) const
{
    FPlatformMemory::Memcpy(Out, AnsiName, sizeof(ANSICHAR) * Header.Len);
	Out[Header.Len] = '\0';
}

void FNameEntry::CopyUnterminatedName(WIDECHAR* Out) const
{
    FPlatformMemory::Memcpy(Out, WideName, sizeof(WIDECHAR) * Header.Len);
	Out[Header.Len] = '\0';
}

void FNameEntry::GetAnsiName(ANSICHAR(&Out)[NAME_SIZE]) const
{
    assert(!IsWide());
    CopyUnterminatedName(Out);
    Out[Header.Len] = '\0';
}

void FNameEntry::GetWideName(WIDECHAR(&Out)[NAME_SIZE]) const
{
    assert(IsWide());
    CopyUnterminatedName(Out);
    Out[Header.Len] = '\0';
}
*/

/*-----------------------------------------------------------------------------
	FName statics.
-----------------------------------------------------------------------------*/

/*
FNameEntry const* FName::GetEntry(FNameEntryId Id)
{
	// Public interface, recurse to the actual string entry if necessary
	return ResolveEntryRecursive(Id);
}

#if WITH_CASE_PRESERVING_NAME
FNameEntryId FName::GetComparisonIdFromDisplayId(FNameEntryId DisplayId)
{
	return GetEntry(DisplayId)->ComparisonId;
}
#endif

// Resolve the entry directly referred to by LookupId
const FNameEntry* FName::ResolveEntry(FNameEntryId LookupId)
{
	return &GetNamePool().Resolve(LookupId);
}

// Recursively resolve through the entry referred to by LookupId to reach the allocated string entry, in the case of UE_FNAME_OUTLINE_NUMBER=1
const FNameEntry* FName::ResolveEntryRecursive(FNameEntryId LookupId)
{
	const FNameEntry* Entry = ResolveEntry(LookupId);
#if UE_FNAME_OUTLINE_NUMBER
	if (Entry->Header.Len == 0)
	{
		return ResolveEntry(Entry->GetNumberedName().Id); // Should only ever recurse one level
	}
	else
#endif
	{
		return Entry;
	}
}
*/

struct FNameHelper
{
	template <typename CharType>
	static FName MakeFName(const CharType* Str)
	{
		if constexpr (std::is_same_v<CharType, char>)
		{
			return MakeFName(Str, static_cast<uint32>(strlen(Str)));
		}
		else if constexpr (std::is_same_v<CharType, wchar_t>)
		{
			return MakeFName(Str, static_cast<uint32>(wcslen(Str)));
		}
		else
		{
			static_assert(false, "Invalid Character type");
			return {};
		}
	}

	template <typename CharType>
	static FName MakeFName(const CharType* Char, uint32 Len)
	{
		// 문자열의 길이가 NAME_SIZE를 초과하면 None 반환
		if (Len >= NAME_SIZE)
		{
			return {};
		}

		const FNameEntryId DisplayId = FNamePool::Get().Store({ Char, Len });
		//UE_LOG("NumEntries in FNamePool: %d", FNamePool::Get().NumEntries())

		FName Result;
		Result.DisplayIndex = DisplayId.Value;
		Result.ComparisonIndex = ResolveComparisonId(DisplayId).Value;
		return Result;
	}

	static FNameEntryId ResolveComparisonId(FNameEntryId DisplayId)
	{
		if (DisplayId.IsNone())
		{
			return {};
		}
		return FNamePool::Get().Resolve(DisplayId.Value).ComparisonId;
	}
};

FName::FName(const WIDECHAR* Name)
	: FName(FNameHelper::MakeFName(Name))
{}

FName::FName(const ANSICHAR* Name)
	: FName(FNameHelper::MakeFName(Name))
{}

FName::FName(const FString& Name)
	: FName(FNameHelper::MakeFName(*Name, Name.Len()))
{}

FString FName::ToString() const
{
	if (DisplayIndex == 0 && ComparisonIndex == 0)
	{
		return { TEXT("None") };
	}

	FNameEntry Entry = FNamePool::Get().Resolve(DisplayIndex);
	return {
#if IS_WIDECHAR
		Entry.WideName
#else
		Entry.AnsiName
#endif
	};
}
