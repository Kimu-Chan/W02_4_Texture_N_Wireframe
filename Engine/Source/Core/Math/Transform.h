#pragma once

#include "Vector.h"
#include "Matrix.h"
#include "Plane.h"
#include "Engine/Engine.h"

#define TORAD 0.0174532925199432957f

struct FTransform
{
protected:
	FVector Position;
	FQuat Rotation;
	FVector Scale;
	int Depth;
	
public:
	FTransform()
		: Position(FVector(0, 0, 0))
		, Rotation(FQuat(0, 0, 0, 1))
		, Scale(FVector(1, 1, 1))
	{
	}

	FTransform(FVector InPosition, FVector InRotation, FVector InScale)
		: Position(InPosition)
		, Rotation(InRotation)
		, Scale(InScale)
	{
	}

	FTransform(FVector InPosition, FQuat InQuat, FVector InScale)
		: Position(InPosition)
		, Rotation(InQuat)
		, Scale(InScale)
	{
	}
	        
	inline FMatrix GetViewMatrix() const
	{
		return FMatrix::LookAtLH(Position, Position + GetForward(), GetUp());
	}
	
	inline virtual void SetPosition(const FVector& InPosition)
	{
		Position = InPosition;
	}

	inline virtual void SetPosition(float x, float y, float z)
	{
		Position = {x, y, z};
	}

	inline virtual void SetRotation(const FVector& InRotation)
	{
		Rotation = FQuat::EulerToQuaternion(InRotation);
	}

	inline virtual void SetRotation(float x, float y, float z)
	{
		SetRotation(FVector(x, y, z));
	}

	inline void SetScale(FVector InScale)
	{
		Scale = InScale;
	}

	inline void AddScale(FVector InScale)
	{
		Scale.X += InScale.X;
		Scale.Y += InScale.Y;
		Scale.Z += InScale.Z;
	}

	inline void SetScale(float x, float y, float z)
	{
		Scale = {x, y, z};
	}

	FVector GetPosition() const
	{
		return Position;
	}

	FQuat GetRotation() const 
	{
		return Rotation;
	}

	FVector GetScale() const
	{
		return Scale;
	}

	FMatrix GetMatrix() const 
	{
		/*return FMatrix::GetScaleMatrix(Scale.X, Scale.Y, Scale.Z)
			* FMatrix::GetRotateMatrix(Rotation)
			* FMatrix::GetTranslateMatrix(Position.X, Position.Y, Position.Z);*/

		FVector X = Rotation.RotateVector(FVector(1, 0, 0));
		FVector Y = Rotation.RotateVector(FVector(0, 1, 0));
		FVector Z = Rotation.RotateVector(FVector(0, 0, 1));

		X = X * Scale.X;
		Y = Y * Scale.Y;
		Z = Z * Scale.Z;

		FVector T = Position;

		return FMatrix{
			X.X, X.Y, X.Z, 0,
			Y.X, Y.Y, Y.Z, 0,
			Z.X, Z.Y, Z.Z, 0,
			T.X, T.Y, T.Z, 1
		};
	}

	FVector GetForward() const
	{
		// 쿼터니언을 회전 행렬로 변환
		FMatrix RotationMatrix = FMatrix::GetRotateMatrix(Rotation);

		// 회전 행렬의 첫 번째 열이 Forward 벡터를 나타냄
		FVector Forward = FVector(
			RotationMatrix.M[0][0],
			RotationMatrix.M[1][0],
			RotationMatrix.M[2][0]
		);

		return Forward.GetSafeNormal();
	}

	FVector GetRight() const
	{
		return FVector::CrossProduct(FVector(0, 0, 1), GetForward()).GetSafeNormal();
	}

	FVector GetUp() const{
		return FVector::CrossProduct(GetForward(), GetRight()).GetSafeNormal();

	}

	void Translate(const FVector& InTranslation)
	{
		Position += InTranslation;
	}

	// InRotate는 Degree 단위
	void Rotate(const FVector& InRotation)
	{
		RotateYaw(InRotation.Z);
		RotatePitch(InRotation.Y);
		RotateRoll(InRotation.X);
	}

	void RotateYaw(float Angle)
	{
		FVector Axis = FVector(0, 0, 1);
		Rotation = FQuat::MultiplyQuaternions(FQuat(Axis, Angle), Rotation);

		//Rotation = FQuat::MultiplyQuaternions(Rotation, FQuat(0, 0, sin(Angle * TORAD / 2), cos(Angle * TORAD / 2)));
	}

	void RotatePitch(float Angle)
	{
		FVector Axis = FVector(0, 1, 0).GetSafeNormal();
		Rotation = FQuat::MultiplyQuaternions(FQuat(Axis, Angle), Rotation);
	}

	void RotateRoll(float Angle)
	{
		FVector Axis = FVector(1, 0, 0).GetSafeNormal();
		Rotation = FQuat::MultiplyQuaternions(FQuat(Axis, Angle), Rotation);
	}

	FTransform operator*(const FTransform& InTransform) const
	{

		//FMatrix TR = FMatrix::GetTranslateMatrix(Position) * FMatrix::GetRotateMatrix(Rotation);
		//FMatrix InTR = FMatrix::GetTranslateMatrix(InTransform.Position) * FMatrix::GetRotateMatrix(InTransform.Rotation);

		//FMatrix ScaleMat = FMatrix::GetScaleMatrix(Scale) * FMatrix::GetScaleMatrix(InTransform.Scale);
		//FTransform NewTransform  = (TR * InTR * ScaleMat).GetTransform();

		//return NewTransform;
		//
		FTransform NewTransform;

		NewTransform.Position = Position + (Rotation.RotateVector(Scale * InTransform.Position));
		NewTransform.Rotation = FQuat::MultiplyQuaternions(Rotation, InTransform.Rotation);
		NewTransform.Scale = Scale * InTransform.Scale;

		return NewTransform; 
	}
};