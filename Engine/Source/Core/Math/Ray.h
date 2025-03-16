#pragma once
#include "Vector.h"

struct FRay
{
public:
	FRay() = default;
	FRay(const FVector& InOrigin, const FVector& InDirection, float InLength)
		: Origin(InOrigin), Direction(InDirection), Length(InLength)
	{
	}
	FVector Origin;
	FVector Direction;
	float Length;
	static FRay GetRayByMousePoint(class ACamera* InCamera);
};