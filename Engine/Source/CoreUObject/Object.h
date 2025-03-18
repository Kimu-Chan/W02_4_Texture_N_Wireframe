#pragma once

#include "NameTypes.h"
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

	FName NamePrivate;

public:
	UObject();
	virtual ~UObject();

public:
	uint32 GetUUID() const { return UUID; }
	uint32 GetInternalIndex() const { return InternalIndex; }

	FName GetFName() const { return NamePrivate; }
	FString GetName() const { return GetFName().ToString(); }
	void SetName(const FString& InName) { NamePrivate = InName; }

public:
	template<typename T>
	bool IsA()
	{
		return GetClass()->IsA<T>();
	}
};