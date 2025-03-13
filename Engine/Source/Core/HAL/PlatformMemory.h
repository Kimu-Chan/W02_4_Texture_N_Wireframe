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

