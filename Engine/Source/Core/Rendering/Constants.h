#pragma once
struct FMatrix;
struct FVector;
#include "Engine/Engine.h"

// 한 프레임 동안 렌더할 각 오브젝트마다 바뀌는 값
struct alignas(16) FCbChangeEveryObject
{
	FMatrix WorldMatrix;
	FVector4 CustomColor;
	// true인 경우 Vertex Color를 사용하고, false인 경우 CustomColor를 사용합니다.
	uint32 bUseVertexColor;
};

// 한 프레임에 한번 바뀌는 값
struct alignas(16) FCbChangeOnCameraMove
{
	FMatrix ViewMatrix;
	FVector ViewPosition;
};

// 화면 크기가 바뀌거나 FOV값이 바뀌는 특정 상황에만 바뀌는 값
struct alignas(16) FCbChangeOnResizeAndFov
{
	FMatrix ProjectionMatrix;
	float NearClip;
	float FarClip;
};

struct alignas(16) FPickingConstants
{
	FVector4 UUIDColor;
};

struct alignas(16) FDepthConstants {
	unsigned int DepthOffset;
	int nearPlane;
	int farPlane;
};

struct alignas(16) FTextureConstants
{
	FMatrix WorldViewProj;
	float U;
	float V;
	float Cols;
	float Rows;
};

struct ConstantUpdateInfo
{
	const FMatrix& TransformMatrix;
	const FVector4& Color;
	bool bUseVertexColor;
};

struct FVertexGrid
{
	FVector Location;
};