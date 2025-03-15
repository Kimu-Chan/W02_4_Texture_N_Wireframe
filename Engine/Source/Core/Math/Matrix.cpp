#include "pch.h" 
#include "Vector.h"
#include "Matrix.h"
#include "Plane.h"
#include "Transform.h"

const FMatrix FMatrix::Identity = FMatrix(  1, 0, 0, 0,
                                            0, 1, 0, 0,
                                            0, 0, 1, 0,
                                            0, 0, 0, 1);

FMatrix::FMatrix()
{
        // identity matrix
        M[0][0] = 1.0f; M[0][1] = 0.0f; M[0][2] = 0.0f; M[0][3] = 0.0f;
        M[1][0] = 0.0f; M[1][1] = 1.0f; M[1][2] = 0.0f; M[1][3] = 0.0f;
        M[2][0] = 0.0f; M[2][1] = 0.0f; M[2][2] = 1.0f; M[2][3] = 0.0f;
        M[3][0] = 0.0f; M[3][1] = 0.0f; M[3][2] = 0.0f; M[3][3] = 1.0f;
}

FMatrix::FMatrix(const FMatrix& other)
{
        M[0][0] = other.M[0][0]; M[0][1] = other.M[0][1];  M[0][2] = other.M[0][2];  M[0][3] = other.M[0][3];
        M[1][0] = other.M[1][0]; M[1][1] = other.M[1][1];  M[1][2] = other.M[1][2];  M[1][3] = other.M[1][3];
        M[2][0] = other.M[2][0]; M[2][1] = other.M[2][1];  M[2][2] = other.M[2][2];  M[2][3] = other.M[2][3];
        M[3][0] = other.M[3][0]; M[3][1] = other.M[3][1];  M[3][2] = other.M[3][2];  M[3][3] = other.M[3][3];
}

FMatrix::FMatrix(const FVector4& InX, const FVector4& InY, const FVector4& InZ, const FVector4& InW)
{
        // row major
        M[0][0] = InX.X; M[0][1] = InX.Y; M[0][2] = InX.Z; M[0][3] = InX.W;
        M[1][0] = InY.X; M[1][1] = InY.Y; M[1][2] = InY.Z; M[1][3] = InY.W;
        M[2][0] = InZ.X; M[2][1] = InZ.Y; M[2][2] = InZ.Z; M[2][3] = InZ.W;
        M[3][0] = InW.X; M[3][1] = InW.Y; M[3][2] = InW.Z; M[3][3] = InW.W;
}

FMatrix::FMatrix(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33)
{
        M[0][0] = m00; M[0][1] = m01;  M[0][2] = m02;  M[0][3] = m03;
        M[1][0] = m10; M[1][1] = m11;  M[1][2] = m12;  M[1][3] = m13;
        M[2][0] = m20; M[2][1] = m21;  M[2][2] = m22;  M[2][3] = m23;
        M[3][0] = m30; M[3][1] = m31;  M[3][2] = m32;  M[3][3] = m33;
}

static constexpr float PIDIV4 = PI / 4.0f;

FMatrix FMatrix::operator+(const FMatrix& Other) const
{
        FMatrix Result;
        for (int i = 0; i < 4; i++)
        {
                for (int j = 0; j < 4; j++)
                {
                        Result.M[i][j] = M[i][j] + Other.M[i][j];
                }
        }
        return Result;
}

FMatrix FMatrix::operator+=(const FMatrix& Other)
{
        *this = *this + Other;
        return *this;
}

FMatrix FMatrix::operator-(const FMatrix& Other) const
{
        FMatrix Result;
        for (int i = 0; i < 4; i++)
        {
                for (int j = 0; j < 4; j++)
                {
                        Result.M[i][j] = M[i][j] - Other.M[i][j];
                }
        }
        return Result;
}

FMatrix FMatrix::operator-=(const FMatrix& Other)
{
        *this = *this - Other;
        return *this;
}

FMatrix FMatrix::operator*(const FMatrix& Other) const
{
        FMatrix Result;
        for (int i = 0; i < 4; i++)
        {
                for (int j = 0; j < 4; j++)
                {
                        Result.M[i][j] = M[i][0] * Other.M[0][j] +
                                M[i][1] * Other.M[1][j] +
                                M[i][2] * Other.M[2][j] +
                                M[i][3] * Other.M[3][j];
                }
        }
        return Result;
}

FMatrix FMatrix::operator*=(const FMatrix& Other)
{
        *this = *this * Other;
        return *this;
}

FMatrix FMatrix::operator*(float Other) const
{
        FMatrix Result;
        for (int i = 0; i < 4; i++)
        {
                for (int j = 0; j < 4; j++)
                {
                        Result.M[i][j] = M[i][j] * Other;
                }
        }
        return Result;
}

FMatrix FMatrix::operator*=(float Other)
{
        *this = *this * Other;
        return *this;
}

bool FMatrix::operator==(const FMatrix& Other) const
{
        bool bEqual = true;
        for (int i = 0; i < 4; i++)
        {
                for (int j = 0; j < 4; j++)
                {
                        if (M[i][j] != Other.M[i][j])
                        {
                                bEqual = false;
                                break;
                        }
                }
        }

        return bEqual;
}

bool FMatrix::operator!=(const FMatrix& Other) const
{
        return !(*this == Other);
}

FMatrix FMatrix::GetTransposed() const
{
        FMatrix Result;
        for (int i = 0; i < 4; i++)
        {
                for (int j = 0; j < 4; j++)
                {
                        Result.M[i][j] = M[j][i];
                }
        }
        return Result;
}

float FMatrix::Determinant() const
{
        const float* m = &M[0][0];
        return
                m[0] * (m[5] * (m[10] * m[15] - m[11] * m[14]) - m[6] * (m[9] * m[15] - m[11] * m[13]) + m[7] * (m[9] * m[14] - m[10] * m[13])) -
                m[1] * (m[4] * (m[10] * m[15] - m[11] * m[14]) - m[6] * (m[8] * m[15] - m[11] * m[12]) + m[7] * (m[8] * m[14] - m[10] * m[12])) +
                m[2] * (m[4] * (m[9] * m[15] - m[11] * m[13]) - m[5] * (m[8] * m[15] - m[11] * m[12]) + m[7] * (m[8] * m[13] - m[9] * m[12])) -
                m[3] * (m[4] * (m[9] * m[14] - m[10] * m[13]) - m[5] * (m[8] * m[14] - m[10] * m[12]) + m[6] * (m[8] * m[13] - m[9] * m[12]));
}

FMatrix FMatrix::Inverse() const
{
        const float Det = Determinant();
        if (FMath::Abs(Det) < 1.0e-6f)
        {
                return {};
        }

        FMatrix Result;
        const float* m = &M[0][0];
        const float InvDet = 1.0f / Det;

        Result.M[0][0] = InvDet * (m[5] * (m[10] * m[15] - m[11] * m[14]) - m[6] * (m[9] * m[15] - m[11] * m[13]) + m[7] * (m[9] * m[14] - m[10] * m[13]));
        Result.M[0][1] = -InvDet * (m[1] * (m[10] * m[15] - m[11] * m[14]) - m[2] * (m[9] * m[15] - m[11] * m[13]) + m[3] * (m[9] * m[14] - m[10] * m[13]));
        Result.M[0][2] = InvDet * (m[1] * (m[6] * m[15] - m[7] * m[14]) - m[2] * (m[5] * m[15] - m[7] * m[13]) + m[3] * (m[5] * m[14] - m[6] * m[13]));
        Result.M[0][3] = -InvDet * (m[1] * (m[6] * m[11] - m[7] * m[10]) - m[2] * (m[5] * m[11] - m[7] * m[9]) + m[3] * (m[5] * m[10] - m[6] * m[9]));

        Result.M[1][0] = -InvDet * (m[4] * (m[10] * m[15] - m[11] * m[14]) - m[6] * (m[8] * m[15] - m[11] * m[12]) + m[7] * (m[8] * m[14] - m[10] * m[12]));
        Result.M[1][1] = InvDet * (m[0] * (m[10] * m[15] - m[11] * m[14]) - m[2] * (m[8] * m[15] - m[11] * m[12]) + m[3] * (m[8] * m[14] - m[10] * m[12]));
        Result.M[1][2] = -InvDet * (m[0] * (m[6] * m[15] - m[7] * m[14]) - m[2] * (m[4] * m[15] - m[7] * m[12]) + m[3] * (m[4] * m[14] - m[6] * m[12]));
        Result.M[1][3] = InvDet * (m[0] * (m[6] * m[11] - m[7] * m[10]) - m[2] * (m[4] * m[11] - m[7] * m[8]) + m[3] * (m[4] * m[10] - m[6] * m[8]));

        Result.M[2][0] = InvDet * (m[4] * (m[9] * m[15] - m[11] * m[13]) - m[5] * (m[8] * m[15] - m[11] * m[12]) + m[7] * (m[8] * m[13] - m[9] * m[12]));
        Result.M[2][1] = -InvDet * (m[0] * (m[9] * m[15] - m[11] * m[13]) - m[1] * (m[8] * m[15] - m[11] * m[12]) + m[3] * (m[8] * m[13] - m[9] * m[12]));
        Result.M[2][2] = InvDet * (m[0] * (m[5] * m[15] - m[7] * m[13]) - m[1] * (m[4] * m[15] - m[7] * m[12]) + m[3] * (m[4] * m[13] - m[5] * m[12]));
        Result.M[2][3] = -InvDet * (m[0] * (m[5] * m[11] - m[7] * m[9]) - m[1] * (m[4] * m[11] - m[7] * m[8]) + m[3] * (m[4] * m[9] - m[5] * m[8]));

        Result.M[3][0] = -InvDet * (m[4] * (m[9] * m[14] - m[10] * m[13]) - m[5] * (m[8] * m[14] - m[10] * m[12]) + m[6] * (m[8] * m[13] - m[9] * m[12]));
        Result.M[3][1] = InvDet * (m[0] * (m[9] * m[14] - m[10] * m[13]) - m[1] * (m[8] * m[14] - m[10] * m[12]) + m[2] * (m[8] * m[13] - m[9] * m[12]));
        Result.M[3][2] = -InvDet * (m[0] * (m[5] * m[14] - m[6] * m[13]) - m[1] * (m[4] * m[14] - m[6] * m[12]) + m[2] * (m[4] * m[13] - m[5] * m[12]));
        Result.M[3][3] = InvDet * (m[0] * (m[5] * m[10] - m[6] * m[9]) - m[1] * (m[4] * m[10] - m[6] * m[8]) + m[2] * (m[4] * m[9] - m[5] * m[8]));

        return Result;
}

FMatrix FMatrix::Transpose(const FMatrix& Matrix)
{
        FMatrix Result;
        for (int i = 0; i < 4; ++i)
        {
                for (int j = 0; j < 4; ++j)
                {
                        Result.M[i][j] = Matrix.M[j][i];
                }
        }
        return Result;
}

FMatrix FMatrix::GetTranslateMatrix(float X, float Y, float Z)
{
        FMatrix Result;
        Result.M[3][0] = X;
        Result.M[3][1] = Y;
        Result.M[3][2] = Z;
        return Result;
}

FMatrix FMatrix::GetTranslateMatrix(FVector Translation)
{
        return GetTranslateMatrix(Translation.X, Translation.Y, Translation.Z);
}

FMatrix FMatrix::GetScaleMatrix(float X, float Y, float Z)
{
        FMatrix Result;
        Result.M[0][0] = X;
        Result.M[1][1] = Y;
        Result.M[2][2] = Z;
        return Result;
}

FMatrix FMatrix::GetScaleMatrix(const FVector& InScale)
{
        return GetScaleMatrix(InScale.X, InScale.Y, InScale.Z);
}

FMatrix FMatrix::GetRotateMatrix(const FQuat& Q)
{
        // 쿼터니언 요소 추출
        const float x = Q.X, y = Q.Y, z = Q.Z, w = Q.W;

        // 중간 계산값
        const float xx = x * x, yy = y * y, zz = z * z;
        const float xy = x * y, xz = x * z, yz = y * z;
        const float wx = w * x, wy = w * y, wz = w * z;

        // 회전 행렬 구성
        FMatrix Result;

        Result.M[0][0] = 1.0f - 2.0f * (yy + zz);
        Result.M[0][1] = 2.0f * (xy - wz);
        Result.M[0][2] = 2.0f * (xz + wy);
        Result.M[0][3] = 0.0f;

        Result.M[1][0] = 2.0f * (xy + wz);
        Result.M[1][1] = 1.0f - 2.0f * (xx + zz);
        Result.M[1][2] = 2.0f * (yz - wx);
        Result.M[1][3] = 0.0f;

        Result.M[2][0] = 2.0f * (xz - wy);
        Result.M[2][1] = 2.0f * (yz + wx);
        Result.M[2][2] = 1.0f - 2.0f * (xx + yy);
        Result.M[2][3] = 0.0f;

        Result.M[3][0] = 0.0f;
        Result.M[3][1] = 0.0f;
        Result.M[3][2] = 0.0f;
        Result.M[3][3] = 1.0f; // 4x4 행렬이므로 마지막 값은 1

        return Result;
}

FMatrix FMatrix::GetOrthonormalizedMatrix(const FMatrix& Matrix)
{
	FVector Forward = FVector(Matrix.M[0][0], Matrix.M[0][1], Matrix.M[0][2]);
	FVector Right = FVector(Matrix.M[1][0], Matrix.M[1][1], Matrix.M[1][2]);
	FVector Up = FVector(Matrix.M[2][0], Matrix.M[2][1], Matrix.M[2][2]);
	FVector Position = FVector(Matrix.M[3][0], Matrix.M[3][1], Matrix.M[3][2]);

    Forward = Forward.GetSafeNormal();
	Up = FVector::CrossProduct(Forward, Right).GetSafeNormal();
	Right = FVector::CrossProduct(Up, Forward);

	FMatrix Result = FMatrix(
		Forward.X, Forward.Y, Forward.Z, 0.0f,
		Right.X, Right.Y, Right.Z, 0.0f,
		Up.X, Up.Y, Up.Z, 0.0f,
		Position.X, Position.Y, Position.Z, 1.0f
	);
	return Result;

}

/// <summary>
/// 뷰 변환 행렬을 생성합니다.
/// </summary>
/// <param name="EyePosition">카메라의 포지션입니다.</param>
/// <param name="FocusPoint">카메라가 바라보는 곳의 포지션입니다.</param>
/// <param name="UpDirection">카메라의 위쪽 방향입니다.</param>
/// <returns>뷰 변환 행렬을 반환합니다.</returns>
FMatrix FMatrix::LookAtLH(const FVector& EyePosition, const FVector& FocusPoint, const FVector& WorldUp)
{
        FVector Forward = (FocusPoint - EyePosition).GetSafeNormal();
        FVector Right = FVector::CrossProduct(WorldUp, Forward).GetSafeNormal();
        FVector Up = FVector::CrossProduct(Forward, Right).GetSafeNormal();

        // row major
        FMatrix Result = FMatrix(
                FVector4(Right.X, Up.X, Forward.X, 0.0f),
                FVector4(Right.Y, Up.Y, Forward.Y, 0.0f),
                FVector4(Right.Z, Up.Z, Forward.Z, 0.0f),
                FVector4(-Right.Dot(EyePosition), -Up.Dot(EyePosition), -Forward.Dot(EyePosition), 1.0f)
        );

        return Result;
}

FMatrix FMatrix::PerspectiveFovLH(float FieldOfView, float AspectRatio, float NearPlane, float FarPlane)
{
        FMatrix Result;
        float YScale = 1.0f / tan(FieldOfView / 2.0f);
        float XScale = YScale / AspectRatio;
        Result.M[0][0] = XScale;
        Result.M[1][1] = YScale;
        Result.M[2][2] = FarPlane / (FarPlane - NearPlane);
        Result.M[2][3] = 1.0f;
        Result.M[3][2] = -NearPlane * FarPlane / (FarPlane - NearPlane);
        Result.M[3][3] = 0.0f;
        return Result;
}

FMatrix FMatrix::OrthoLH(float ScreenWidth, float ScreenHeight, float NearPlane, float FarPlane)
{
    FMatrix Result;
    Result.M[0][0] = 2.0f / ScreenWidth;
    Result.M[1][1] = 2.0f / ScreenHeight;
    Result.M[2][2] = 1.0f / (FarPlane - NearPlane);
    Result.M[2][3] = 0.0f;
    Result.M[3][2] = -NearPlane / (FarPlane - NearPlane);
    Result.M[3][3] = 1.0f;
    return Result;
}

FVector FMatrix::GetTranslation() const
{
        return FVector(M[3][0], M[3][1], M[3][2]);
}

FVector FMatrix::GetScale() const
{
        float X = FVector(M[0][0], M[0][1], M[0][2]).Length();
        float Y = FVector(M[1][0], M[1][1], M[1][2]).Length();
        float Z = FVector(M[2][0], M[2][1], M[2][2]).Length();
        return { X, Y, Z };

        //return FVector(M[0][0], M[1][1], M[2][2]);
}

FVector FMatrix::GetRotation() const
{
        FQuat Q = FQuat::MakeFromRotationMatrix(*this);

        FVector Euler = Q.GetEuler();
        return Euler;
}

FVector4 FMatrix::TransformVector4(const FVector4& Vector) const
{
        return 
        {
            Vector.X * M[0][0] + Vector.Y * M[1][0] + Vector.Z * M[2][0] + Vector.W * M[3][0],
            Vector.X * M[0][1] + Vector.Y * M[1][1] + Vector.Z * M[2][1] + Vector.W * M[3][1],
            Vector.X * M[0][2] + Vector.Y * M[1][2] + Vector.Z * M[2][2] + Vector.W * M[3][2],
            Vector.X * M[0][3] + Vector.Y * M[1][3] + Vector.Z * M[2][3] + Vector.W * M[3][3]
        };
}

FTransform FMatrix::GetTransform() const
{
        FQuat RotationQuat = FQuat::MakeFromRotationMatrix(*this);
        return FTransform(GetTranslation(), RotationQuat, GetScale());
}

FVector FMatrix::TransformPosition(const FVector& Position) const
{
    FVector4 Result = FMatrix::TransformVector4(FVector4(Position.X, Position.Y, Position.Z, 1.0f));
    return FVector(Result.X, Result.Y, Result.Z);
}

FMatrix FMatrix::GetTransformMatrix() const
{
    FMatrix Translation;
    Translation.M[3][0] = M[3][0];
    Translation.M[3][1] = M[3][1];
    Translation.M[3][2] = M[3][2];

    return Translation;
}

FMatrix FMatrix::GetRotationMatrix() const
{
    FMatrix Rotation;
    FVector Scale = GetScale();

    if (FMath::Abs(Scale.X) < SMALL_NUMBER) Scale.X = 1.0f;
    if (FMath::Abs(Scale.Y) < SMALL_NUMBER) Scale.Y = 1.0f;
    if (FMath::Abs(Scale.Z) < SMALL_NUMBER) Scale.Z = 1.0f;

    Rotation.M[0][0] = M[0][0] / Scale.X;
    Rotation.M[0][1] = M[0][1] / Scale.X;
    Rotation.M[0][2] = M[0][2] / Scale.X;

    Rotation.M[1][0] = M[1][0] / Scale.Y;
    Rotation.M[1][1] = M[1][1] / Scale.Y;
    Rotation.M[1][2] = M[1][2] / Scale.Y;

    Rotation.M[2][0] = M[2][0] / Scale.Z;
    Rotation.M[2][1] = M[2][1] / Scale.Z;
    Rotation.M[2][2] = M[2][2] / Scale.Z;

    return Rotation;
}

FMatrix FMatrix::GetScaleMatrix() const
{
    FMatrix Scale;
    Scale.M[0][0] = M[0][0];
    Scale.M[1][1] = M[1][1];
    Scale.M[2][2] = M[2][2];

    return Scale;
}

float FMatrix::GetMaxScaleFactor() const
{
    float MaxScale = M[0][0];
	MaxScale = FMath::Max(MaxScale, M[1][1]);
	MaxScale = FMath::Max(MaxScale, M[2][2]);

    return MaxScale;
}
