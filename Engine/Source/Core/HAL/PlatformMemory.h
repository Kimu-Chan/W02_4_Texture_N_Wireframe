#pragma once

/*
 * Unreal Engine의 HAL/PlatformMemory.h를 목표로 하는 헤더
 *
#include "CoreTypes.h"
#include "GenericPlatform/GenericPlatformMemory.h"

#include COMPILED_PLATFORM_HEADER(PlatformMemory.h)
 */

#define MULTI_THREAD

#include "PlatformType.h"

enum EAllocationType : uint8
{
	EAT_Object,
	EAT_Container
};

/**
 * 엔진의 Heap 메모리의 할당량을 추적하는 클래스
 * malloc, free 할때마다 메모리 사용량을 추적합니다.
 * @note new로 생성한 객체는 추적하지 않습니다.
 */
struct FPlatformMemory
{
private:
#ifdef MULTI_THREAD
	static std::atomic<uint64> ObjectAllocationBytes;
	static std::atomic<uint64> ObjectAllocationCount;
	static std::atomic<uint64> ContainerAllocationBytes;
	static std::atomic<uint64> ContainerAllocationCount;
#else
	static std::atomic<uint64> TotalAllocationBytes;
	static std::atomic<uint64> TotalAllocationCount;
#endif

	template <EAllocationType AllocType>
	static void IncrementStats(SIZE_T Size);

	template <EAllocationType AllocType>
	static void DecrementStats(SIZE_T Size);

public:
	template <EAllocationType AllocType>
	static void* Malloc(size_t Size);

	template <EAllocationType AllocType>
	static void* AlignedMalloc(size_t Size, size_t Alignment);

	template <EAllocationType AllocType>
	static void Free(void* Address, size_t Size);

	template <EAllocationType AllocType>
	static void AlignedFree(void* Address, size_t Size);

	template <EAllocationType AllocType>
	static uint64 GetAllocationBytes();

	template <EAllocationType AllocType>
	static uint64 GetAllocationCount();

	/** Copies count bytes of characters from Src to Dest. If some regions of the source
	 * area and the destination overlap, memmove ensures that the original source bytes
	 * in the overlapping region are copied before being overwritten.  NOTE: make sure
	 * that the destination buffer is the same size or larger than the source buffer!
	 */
	static FORCEINLINE void* Memmove(void* Dest, const void* Src, SIZE_T Count)
	{
		return memmove(Dest, Src, Count);
	}

	static FORCEINLINE int32 Memcmp(const void* Buf1, const void* Buf2, SIZE_T Count)
	{
		return memcmp(Buf1, Buf2, Count);
	}

	static FORCEINLINE void* Memset(void* Dest, uint8 Char, SIZE_T Count)
	{
		return memset(Dest, Char, Count);
	}

	static FORCEINLINE void* Memzero(void* Dest, SIZE_T Count)
	{
		return memset(Dest, 0, Count);
	}

	static FORCEINLINE void* Memcpy(void* Dest, const void* Src, SIZE_T Count)
	{
		return memcpy(Dest, Src, Count);
	}
};


template <EAllocationType AllocType>
void FPlatformMemory::IncrementStats(size_t Size)
{
#ifdef MULTI_THREAD
	if constexpr (AllocType == EAT_Container)
	{
		ContainerAllocationBytes.fetch_add(Size, std::memory_order_relaxed);
		ContainerAllocationCount.fetch_add(1, std::memory_order_relaxed);
	}
	else if constexpr (AllocType == EAT_Object)
	{
		ObjectAllocationBytes.fetch_add(Size, std::memory_order_relaxed);
		ObjectAllocationCount.fetch_add(1, std::memory_order_relaxed);
	}
	else
	{
		static_assert(false, "Unknown allocation type");
	}
#else
	TotalAllocationBytes += Size;
	++TotalAllocationCount;
#endif
}

template <EAllocationType AllocType>
void FPlatformMemory::DecrementStats(size_t Size)
{
#ifdef MULTI_THREAD
	if constexpr (AllocType == EAT_Container)
	{
		ContainerAllocationBytes.fetch_sub(Size, std::memory_order_relaxed);
		ContainerAllocationCount.fetch_sub(1, std::memory_order_relaxed);
	}
	else if constexpr (AllocType == EAT_Object)
	{
		ObjectAllocationBytes.fetch_sub(Size, std::memory_order_relaxed);
		ObjectAllocationCount.fetch_sub(1, std::memory_order_relaxed);
	}
	else
	{
		static_assert(false, "Unknown allocation type");
	}
#else
	TotalAllocationBytes -= Size;
	--TotalAllocationCount;
#endif
}

template <EAllocationType AllocType>
void* FPlatformMemory::Malloc(const size_t Size)
{
	void* Ptr = std::malloc(Size);
	if (Ptr)
	{
		IncrementStats<AllocType>(Size);
	}
	return Ptr;
}

template <EAllocationType AllocType>
void* FPlatformMemory::AlignedMalloc(const size_t Size, const size_t Alignment)
{
	void* Ptr = _aligned_malloc(Size, Alignment);
	if (Ptr)
	{
		IncrementStats<AllocType>(Size);
	}
	return Ptr;
}

template <EAllocationType AllocType>
void FPlatformMemory::Free(void* Address, const size_t Size)
{
	if (Address)
	{
		DecrementStats<AllocType>(Size);
		std::free(Address);
	}
}

template <EAllocationType AllocType>
void FPlatformMemory::AlignedFree(void* Address, const size_t Size)
{
	if (Address)
	{
		DecrementStats<AllocType>(Size);
		_aligned_free(Address);
	}
}

template <EAllocationType AllocType>
uint64 FPlatformMemory::GetAllocationBytes()
{
#ifdef MULTI_THREAD
	if constexpr (AllocType == EAT_Container)
	{
		return ContainerAllocationBytes;
	}
	else if constexpr (AllocType == EAT_Object)
	{
		return ObjectAllocationBytes;
	}
	else
	{
		static_assert(false, "Unknown AllocationType");
		return -1;
	}
#else
	return TotalAllocationBytes;
#endif
}

template <EAllocationType AllocType>
uint64 FPlatformMemory::GetAllocationCount()
{
#ifdef MULTI_THREAD
	if constexpr (AllocType == EAT_Container)
	{
		return ContainerAllocationCount;
	}
	else if constexpr (AllocType == EAT_Object)
	{
		return ObjectAllocationCount;
	}
	else
	{
		static_assert(false, "Unknown AllocationType");
		return -1;
	}
#else
	return TotalAllocationCount;
#endif
}

