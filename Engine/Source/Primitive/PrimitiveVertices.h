#pragma once

#include "Core/HAL/PlatformType.h"
#include "Rendering/RendererDefine.h"

struct FVertexSimple
{
    float X, Y, Z;    // Position
    float R, G, B, A; // Color
};

struct FVertexUV
{
	float X, Y, Z;    // Position
	float U, V;       // UV
};

extern FVertexSimple LineVertices[2];
extern FVertexSimple CubeVertices[8];
extern FVertexSimple SphereVertices[2400];
extern FVertexSimple TriangleVertices[3];
extern FVertexUV UVQuadVertices[6];