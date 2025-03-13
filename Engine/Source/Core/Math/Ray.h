#pragma once
#include "Vector.h"

struct FRay
{
public:
	FRay() = default;
	FRay(const FVector& InOrigin, const FVector& InDirection)
		: Origin(InOrigin), Direction(InDirection)
	{
	}
	FVector Origin;
	FVector Direction;
	static FRay GetRayByMousePoint(class ACamera* InCamera);
};