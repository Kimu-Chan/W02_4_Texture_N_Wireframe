#pragma once
#include "Vector.h"
#include "Transform.h"

class FBox
{
public:
	FBox() = default;
	FBox(const FVector& InMin, const FVector& InMax)
		: Min(InMin), Max(InMax)
	{
	}
	FVector Min;
	FVector Max;

	FVector InitialMin;
	FVector InitialMax;
	bool bCanBeRendered = false;

	bool IsValid() const
	{
		return Min.X < Max.X && Min.Y < Max.Y && Min.Z < Max.Z;
	}
	bool IntersectRay(const FVector& Origin, const FVector& Dir) const
	{
		float tmin = (Min.X - Origin.X) / Dir.X;
		float tmax = (Max.X - Origin.X) / Dir.X;
		if (tmin > tmax)
		{
			float temp = tmin;
			tmin = tmax;
			tmax = temp;
		}
		float tymin = (Min.Y - Origin.Y) / Dir.Y;
		float tymax = (Max.Y - Origin.Y) / Dir.Y;
		if (tymin > tymax)
		{
			float temp = tymin;
			tymin = tymax;
			tymax = temp;
		}
		if ((tmin > tymax) || (tymin > tmax))
			return false;
		if (tymin > tmin)
			tmin = tymin;
		if (tymax < tmax)
			tmax = tymax;
		float tzmin = (Min.Z - Origin.Z) / Dir.Z;
		float tzmax = (Max.Z - Origin.Z) / Dir.Z;
		if (tzmin > tzmax)
		{
			float temp = tzmin;
			tzmin = tzmax;
			tzmax = temp;
		}
		if ((tmin > tzmax) || (tzmin > tmax))
			return false;
		return true;
	}
	// Transform 변환 시 AABB도 변환
	void Update(const FMatrix& InModelMatrix)
	{
		// 스케일 변환만 해주면된다
		FVector Vertices[8] =
		{
			InModelMatrix.TransformPosition(FVector(InitialMin.X, InitialMin.Y, InitialMin.Z)),
			InModelMatrix.TransformPosition(FVector(InitialMin.X, InitialMin.Y, InitialMax.Z)),
			InModelMatrix.TransformPosition(FVector(InitialMin.X, InitialMax.Y, InitialMin.Z)),
			InModelMatrix.TransformPosition(FVector(InitialMin.X, InitialMax.Y, InitialMax.Z)),
			InModelMatrix.TransformPosition(FVector(InitialMax.X, InitialMin.Y, InitialMin.Z)),
			InModelMatrix.TransformPosition(FVector(InitialMax.X, InitialMin.Y, InitialMax.Z)),
			InModelMatrix.TransformPosition(FVector(InitialMax.X, InitialMax.Y, InitialMin.Z)),
			InModelMatrix.TransformPosition(FVector(InitialMax.X, InitialMax.Y, InitialMax.Z))
		};

		FVector NewMin = Vertices[0];
		FVector NewMax = Vertices[0];

		for (int i = 1; i < 8; i++)
		{
			NewMin.X = FMath::Min(NewMin.X, Vertices[i].X);
			NewMin.Y = FMath::Min(NewMin.Y, Vertices[i].Y);
			NewMin.Z = FMath::Min(NewMin.Z, Vertices[i].Z);
			NewMax.X = FMath::Max(NewMax.X, Vertices[i].X);
			NewMax.Y = FMath::Max(NewMax.Y, Vertices[i].Y);
			NewMax.Z = FMath::Max(NewMax.Z, Vertices[i].Z);
		}

		Min = NewMin;
		Max = NewMax;
	}

	void Init(const FVector& InMin, const FVector& InMax)
	{
		InitialMin = Min = InMin;
		InitialMax = Max = InMax;
	}

	void Init(const FVector& InCenter, float InRadius)
	{
		Min = InCenter - FVector(InRadius, InRadius, InRadius);
		Max = InCenter + FVector(InRadius, InRadius, InRadius);
	}

	FVector GetCenter() const
	{
		return (Min + Max) * 0.5f;
	}

};

