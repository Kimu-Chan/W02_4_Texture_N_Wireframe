#pragma once

#include "Core/HAL/ContainerAllocator.h"
#include "Array.h"

template <typename T, typename Allocator = FDefaultAllocator<T>>
class TSet
{
private:
    using SetType = std::unordered_set<T, std::hash<T>, std::equal_to<T>, Allocator>;
    SetType PrivateSet;

public:
    using SizeType = typename Allocator::SizeType;
    using Iterator = typename SetType::iterator;
    using ConstIterator = typename SetType::const_iterator;

    // 기본 생성자
    TSet() = default;

    // Iterator 관련 메서드
    Iterator begin() noexcept { return PrivateSet.begin(); }
    Iterator end() noexcept { return PrivateSet.end(); }
    ConstIterator begin() const noexcept { return PrivateSet.begin(); }
    ConstIterator end() const noexcept { return PrivateSet.end(); }

    // Add
    void Add(const T& Item) { PrivateSet.insert(Item); }

    // Emplace
    template<typename... Args>
    std::pair<Iterator, bool> Emplace(Args&&... args) 
    { 
        return PrivateSet.emplace(std::forward<Args>(args)...); 
    }

    // Num (개수)
    SizeType Num() const { return static_cast<SizeType>(PrivateSet.size()); }

    // Find
    Iterator Find(const T& Item) { return PrivateSet.find(Item); }
    ConstIterator Find(const T& Item) const { return PrivateSet.find(Item); }

    // Array (TArray로 반환)
    TArray<T, Allocator> Array() const
    {
        TArray<T, Allocator> Result;
        Result.Reserve(Num());
        for (const auto& Item : PrivateSet)
        {
            Result.Add(Item);
        }
        return Result;
    }

    // Remove
    SizeType Remove(const T& Item) { return static_cast<SizeType>(PrivateSet.erase(Item)); }

    // Empty
    void Empty() { PrivateSet.clear(); }

    // IsEmpty
    bool IsEmpty() const { return PrivateSet.empty(); }
};
