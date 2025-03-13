#pragma once

struct FVector4;
struct FVector;
struct FQuat;

struct alignas(16) FMatrix
{
	float M[4][4];
	static const FMatrix Identity;

	FMatrix();
	FMatrix(const FMatrix& other);
	FMatrix(const FVector4& InX, const FVector4& InY, const FVector4& InZ, const FVector4& InW);
	FMatrix(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33);

	inline void SetIdentity()
	{
		M[0][0] = 1; M[0][1] = 0;  M[0][2] = 0;  M[0][3] = 0;
		M[1][0] = 0; M[1][1] = 1;  M[1][2] = 0;  M[1][3] = 0;
		M[2][0] = 0; M[2][1] = 0;  M[2][2] = 1;  M[2][3] = 0;
		M[3][0] = 0; M[3][1] = 0;  M[3][2] = 0;  M[3][3] = 1;
	}

	static FMatrix Transpose(const FMatrix& Matrix);
	static FMatrix GetTranslateMatrix(float X, float Y, float Z);
	static FMatrix GetTranslateMatrix(FVector Translation);
	static FMatrix GetScaleMatrix(float X, float Y, float Z);
	static FMatrix GetScaleMatrix(const FVector& InScale);
	static FMatrix GetRotateMatrix(const FQuat& Q);
	static FMatrix LookAtLH(const FVector& EyePosition, const FVector& FocusPoint, const FVector& WorldUp);
	static FMatrix PerspectiveFovLH(float FieldOfView, float AspectRatio, float NearPlane, float FarPlane);
	static FMatrix OrthoLH(float ScreenWidth, float ScreenHeight, float NearPlane, float FarPlane);

	FMatrix operator+(const FMatrix& Other) const;
	FMatrix operator+=(const FMatrix& Other);
	FMatrix operator-(const FMatrix& Other) const;
	FMatrix operator-=(const FMatrix& Other);
	FMatrix operator*(const FMatrix& Other) const;
	FMatrix operator*=(const FMatrix& Other);
	FMatrix operator*(float Other) const;
	FMatrix operator*=(float Other);
	bool operator==(const FMatrix& Other) const;
	bool operator!=(const FMatrix& Other) const;

	FMatrix GetTransposed() const;
	float Determinant() const;
	FMatrix Inverse() const;

	FVector GetTranslation() const;
	FVector GetScale() const;
	FVector GetRotation() const;

	FVector4 TransformVector4(const FVector4& Vector) const;

	class FTransform GetTransform() const;

	FVector TransformPosition(const FVector& Position) const;
	FMatrix GetTransformMatrix() const;
	FMatrix GetRotationMatrix() const;
	FMatrix GetScaleMatrix() const;
};