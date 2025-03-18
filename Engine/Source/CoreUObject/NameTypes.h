#pragma once
// There is only FName right now, Can be expanded to include other name types

#include "HAL/PlatformType.h"
#include "Container/String.h"

/*----------------------------------------------------------------------------
	Definitions.
----------------------------------------------------------------------------*/

/**
 * FName에 대소문자 구분을 지원할지 여부 (기본: 지원)
 * FName에 추가적인 NAME_INDEX 변수를 추가합니다.
 * 여전히 ToString()은 FName::Init에 전달된 정확한 문자열을 반환합니다. (FNames가 최종 사용자에게 표시되는 경우 유용)
 * Runtime에는 비활성화되어 있습니다. (Unreal Engine 기준)
 */
#ifndef WITH_CASE_PRESERVING_NAME
#define WITH_CASE_PRESERVING_NAME 1
#endif

//@note: 대부분의 경우 FName에 저장하므로 따로 구현하지 않았습니다..!
 // Fname의 숫자 부분을 NameTable에 저장할지 FName 인스턴스에 저장할지 여부 (기본: 0, FName에 저장)
 // NameTable에 저장하면 전체적으로 메모리를 절약할 수 있지만 새로운 숫자 접미사는 고유한 문자열처럼 작용하여 NameTable이 커집니다.
#ifndef UE_FNAME_OUTLINE_NUMBER
#define UE_FNAME_OUTLINE_NUMBER 0
#endif // UE_FNAME_OUTLINE_NUMBER

/** FName에 저장될 수 있는 최대 길이 (공백문자 포함) */
enum { NAME_SIZE = 256 };//1024 };

/** Opaque id to a deduplicated name */
struct FNameEntryId {
//private:
	uint32 Value; // 비교를 위한 문자열의 Hash

public:
	FNameEntryId() : Value(0) {}	// Default값은 NAME_None임
	FNameEntryId(uint32 InValue) : Value(InValue) {}	//TODO: explicit 키워드 추가

	bool IsNone() const	{ return Value == 0; }

	bool operator==(FNameEntryId Rhs) const { return Value == Rhs.Value; }
	bool operator!=(FNameEntryId Rhs) const { return Value != Rhs.Value; }

	explicit operator bool() const { return Value != 0; }	// FNameEntryId가 0(NAME_None)이 아닌 경우 true 반환

	// 현재 Value가 Public이어서 사용하지 않는 함수임
	//@TODO: FNameEntryID를 왜 이렇게 구현했는지 이해해보기
	/** Get process specific integer */
	uint32 ToUnstableInt() const { return Value; }

	/** Create from unstable int produced by this process */
	static FNameEntryId FromUnstableInt(uint32 UnstableInt)
	{
		FNameEntryId Id;
		Id.Value = UnstableInt;
		return Id;
	}
};

/**
 * Legacy typedef - this is no longer an index
 *
 * Use GetTypeHash(FName) or GetTypeHash(FNameEntryId) for hashing
 * To compare with ENames use FName(EName) or FName::ToEName() instead
 */
typedef FNameEntryId NAME_INDEX;

/** Externally, the instance number to represent no instance number is NAME_NO_NUMBER,
	but internally, we add 1 to indices, so we use this #define internally for
	zero'd memory initialization will still make NAME_None as expected */
#define NAME_NO_NUMBER_INTERNAL	0

enum class ENameCase : uint8
{
	CaseSensitive,	// 대소문자 구분
	IgnoreCase,		// 대소문자 무시
};

/** Enumeration for finding name. */
//enum EFindName
//{
//	/**
//	* Find a name; return 0/NAME_None/FName() if it doesn't exist.
//	* When UE_FNAME_OUTLINE_NUMBER is set, we search for the exact name including the number suffix.
//	* Otherwise we search only for the string part.
//	*/
//	FNAME_Find,
//
//	/** Find a name or add it if it doesn't exist. */
//	FNAME_Add
//};

/*----------------------------------------------------------------------------
	FNameEntry.
----------------------------------------------------------------------------*/

/** Entry에 들어갈 Name 정보 */
struct FNameEntryHeader
{
	uint16 bIsWide : 1;	// wchar 여부
#if WITH_CASE_PRESERVING_NAME
	uint16 Len : 15;	// FName의 길이 0 ~ 32767 (2^16 - 1)
#else
	static constexpr inline uint32 ProbeHashBits = 5;
	uint16 LowercaseProbeHash : ProbeHashBits;
	uint16 Len : 10;
#endif
};

/**
 * FNameEntry는 FNamePool에 저장되는 실제 문자열 데이터입니다.
 * 고유한 문자열과 그 문자열의 인스턴스 번호를 관리합니다.
 */
struct FNameEntry {
#if WITH_CASE_PRESERVING_NAME
	FNameEntryId ComparisonId;	// 비교를 위한 Hash
#endif
	FNameEntryHeader Header;	// 문자열 정보

	// Unaligned to reduce alignment waste for non-numbered entries
	struct FNumberedData
	{
#if UE_FNAME_OUTLINE_NUMBER	
#if WITH_CASE_PRESERVING_NAME // ComparisonId is 4B-aligned, 4B-align Id/Number by 2B pad after 2B Header
		uint8 Pad[sizeof(Header) % alignof(decltype(ComparisonId))];
#endif						
		uint8 Id[sizeof(FNameEntryId)];
		uint8 Number[sizeof(uint32)];
#endif // UE_FNAME_OUTLINE_NUMBER	
	};

	union
	{
		ANSICHAR			AnsiName[NAME_SIZE];
		WIDECHAR			WideName[NAME_SIZE];
		//FNumberedData		NumberedName;
	};

private:
	//FNameEntry(const FNameEntry&) = delete;
	//FNameEntry(FNameEntry&&) = delete;
	//FNameEntry& operator=(const FNameEntry&) = delete;
	//FNameEntry& operator=(FNameEntry&&) = delete;

public:
	FORCEINLINE bool IsWide() const { return Header.bIsWide; }
	FORCEINLINE int32 GetNameLength() const { return Header.Len; }
/*
#if UE_FNAME_OUTLINE_NUMBER
	FORCEINLINE bool IsNumbered() const { return Header.Len == 0; }
#else
	FORCEINLINE bool IsNumbered() const { return false; }
#endif
*/
	/** Copy null-terminated name to TCHAR buffer without allocating. */
	void GetName(TCHAR(&OutName)[NAME_SIZE]) const;

	/** Copy null-terminated name to ANSICHAR buffer without allocating. Entry must not be wide. */
	void GetAnsiName(ANSICHAR(&OutName)[NAME_SIZE]) const;

	/** Copy null-terminated name to WIDECHAR buffer without allocating. Entry must be wide. */
	void GetWideName(WIDECHAR(&OutName)[NAME_SIZE]) const;

//private:
	void StoreName(const ANSICHAR* InName, uint32 Len);
	void StoreName(const WIDECHAR* InName, uint32 Len);
	//void CopyUnterminatedName(ANSICHAR* OutName) const;
	//void CopyUnterminatedName(WIDECHAR* OutName) const;

};


/**
 * world에서 사용 가능한 Public Name입니다.\n
 * '고유 문자열 테이블의 인덱스'와 '인스턴스 번호'의 조합으로 저장됩니다.\n
 * 대소문자 구분은 하지 않지만, 대소문자를 보존합니다. (WITH_CASE_PRESERVING_NAME이 1인 경우)
 */
class FName {
	friend struct FNameHelper;
//TODO: uint32를 FNameEntryId로 변경
//private:
	uint32			ComparisonIndex;	// 비교를 위한 Hash
#if !UE_FNAME_OUTLINE_NUMBER
	uint32			Number;			// 숫자 부분 (인스턴스 번호)
#endif// ! //UE_FNAME_OUTLINE_NUMBER
#if WITH_CASE_PRESERVING_NAME
	uint32			DisplayIndex;	// 원본 문자열의 Hash
#endif // WITH_CASE_PRESERVING_NAME

public:
	FNameEntryId GetComparisonIndex() const { return ComparisonIndex; }
	FNameEntryId GetDisplayIndex() const { return DisplayIndex; }
	FString ToString() const;

	FName(const WIDECHAR* Name);
	FName(const ANSICHAR* Name);
	FName(const FString& Name);

	bool operator==(const FName& Other) const {
		return ComparisonIndex == Other.ComparisonIndex;
		//return ComparisonIndex.ToUnstableInt() == Other.ComparisonIndex.ToUnstableInt();
	}

	//explicit FName(FNameEntryId InComparisonIndex, uint32 InNumber)
	//	: ComparisonIndex(InComparisonIndex), Number(InNumber) {
	//}

	/**
	 * Create an FName from its component parts
	 * Only call this if you *really* know what you're doing
	 */
	//FORCEINLINE FName(FNameEntryId InComparisonIndex, FNameEntryId InDisplayIndex, int32 InNumber)
	//	: FName(CreateNumberedNameIfNecessary(InComparisonIndex, InDisplayIndex, InNumber))
	//{
	//}


//#if WITH_CASE_PRESERVING_NAME
//	static FNameEntryId GetComparisonIdFromDisplayId(FNameEntryId DisplayId);
//#else
//	static FNameEntryId GetComparisonIdFromDisplayId(FNameEntryId DisplayId) { return DisplayId; }
//#endif

	/**
	 * Default constructor, initialized to None
	 */
	FName() : DisplayIndex(0), ComparisonIndex(0) {}
//	FORCEINLINE FName()
//#if !UE_FNAME_OUTLINE_NUMBER
//		: Number(NAME_NO_NUMBER_INTERNAL)
//#endif //!UE_FNAME_OUTLINE_NUMBER
//	{}

	/*
	static FNameEntry const* GetEntry(FNameEntryId Id);

	// Resolve the entry directly referred to by LookupId
	static const FNameEntry* ResolveEntry(FNameEntryId LookupId);
	// Recursively resolve through the entry referred to by LookupId to reach the allocated string entry, in the case of UE_FNAME_OUTLINE_NUMBER=1
	static const FNameEntry* ResolveEntryRecursive(FNameEntryId LookupId);

	FORCEINLINE static FName CreateNumberedNameIfNecessary(FNameEntryId ComparisonId, FNameEntryId DisplayId, int32 Number)
	{
#if UE_FNAME_OUTLINE_NUMBER
		if (Number != NAME_NO_NUMBER_INTERNAL)
		{
			// We need to store a new entry in the name table
			return CreateNumberedName(ComparisonId, DisplayId, Number);
		}
		// Otherwise we can just set the index members
#endif
		FName Out;
		Out.ComparisonIndex = ComparisonId;
#if WITH_CASE_PRESERVING_NAME
		Out.DisplayIndex = DisplayId;
#endif
#if !UE_FNAME_OUTLINE_NUMBER
		Out.Number = Number;
#endif
		return Out;
	}
	*/
};

/*
class FDisplayNameEntryId
{
public:
	FDisplayNameEntryId() : FDisplayNameEntryId(FName()) {}
	explicit FDisplayNameEntryId(FName Name) : FDisplayNameEntryId(Name.GetDisplayIndex(), Name.GetComparisonIndex()) {}
	FORCEINLINE FName ToName(uint32 Number) const { return FName(GetComparisonId(), GetDisplayId(), Number); }

private:
#if WITH_CASE_PRESERVING_NAME
	static constexpr uint32 DifferentIdsFlag = 1u << 31;
	static constexpr uint32 DisplayIdMask = ~DifferentIdsFlag;

	uint32 Value = 0;

	FDisplayNameEntryId(FNameEntryId Id, FNameEntryId CmpId) : Value(Id.ToUnstableInt() | (Id != CmpId) * DifferentIdsFlag) {}
	FORCEINLINE bool SameIds() const { return (Value & DifferentIdsFlag) == 0; }
	FORCEINLINE FNameEntryId GetDisplayId() const { return FNameEntryId::FromUnstableInt(Value & DisplayIdMask); }
	FORCEINLINE FNameEntryId GetComparisonId() const { return SameIds() ? GetDisplayId() : FName::GetComparisonIdFromDisplayId(GetDisplayId()); }
	friend bool operator==(FDisplayNameEntryId A, FDisplayNameEntryId B) { return A.Value == B.Value; }
#else
	FNameEntryId Id;

	FDisplayNameEntryId(FNameEntryId InId, FNameEntryId) : Id(InId) {}
	FORCEINLINE FNameEntryId GetDisplayId() const { return Id; }
	FORCEINLINE FNameEntryId GetComparisonId() const { return Id; }
	friend bool operator==(FDisplayNameEntryId A, FDisplayNameEntryId B) { return A.Id == B.Id; }
#endif
	friend bool operator==(FNameEntryId A, FDisplayNameEntryId B) { return A == B.GetDisplayId(); }
	friend bool operator==(FDisplayNameEntryId A, FNameEntryId B) { return A.GetDisplayId() == B; }
	friend uint32 GetTypeHash(FDisplayNameEntryId InId) { return InId.GetDisplayId().ToUnstableInt(); }
};
*/

template <>
struct std::hash<FName>
{
	size_t operator()(const FName& InName) const
	{
		// 해시 계산 로직
		return std::hash<FString::BaseStringType>()(*InName.ToString());
	}
};