#pragma once

#include "Core/HAL/PlatformType.h"
#include "AbstractClass/UClass.h"

class UObject : public UClass
{
public:
	UCLASS(UObject, UClass);

private:
	friend class FObjectFactory;

	uint32 UUID = 0;
	uint32 InternalIndex; // Index of GUObjectArray

public:
	UObject();
	virtual ~UObject();

public:
	uint32 GetUUID() const { return UUID; }
	uint32 GetInternalIndex() const { return InternalIndex; }

public:
	template<typename T>
	bool IsA()
	{
		return GetClass()->IsA<T>();
	}
};