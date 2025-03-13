#include "pch.h" 
#include "BufferCache.h"
#include "Engine/Engine.h"
#include "Primitive/PrimitiveVertices.h"


FBufferCache::FBufferCache()
{
}

FBufferCache::~FBufferCache()
{
}

void FBufferCache::Init()
{

}

VertexBufferInfo FBufferCache::GetBufferInfo(EPrimitiveType Type)
{
    if (!VertexBufferCache.Contains(Type))
    {
        auto bufferInfo = CreateBufferInfo(Type);
		VertexBufferCache.Add(Type, bufferInfo);
    }

    return VertexBufferCache[Type];
}

VertexBufferInfo FBufferCache::CreateBufferInfo(EPrimitiveType Type)
{
    ID3D11Buffer* VertexBuffer = nullptr;
    int VerticeSize = 0;
    D3D_PRIMITIVE_TOPOLOGY Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    FVertexSimple* Vertices = nullptr;

    // !TODO : 모든 프리미티브들에 대해서 동일한 방법의 캐싱 사용
    switch (Type)
    {
    case EPrimitiveType::EPT_Line:
        VerticeSize = std::size(LineVertices);
        Vertices = LineVertices;
        Topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
        break;
    case EPrimitiveType::EPT_Triangle:
        VerticeSize = std::size(TriangleVertices);
        Vertices = TriangleVertices;
        break;
    case EPrimitiveType::EPT_Cube:
        VerticeSize = std::size(CubeVertices);
        Vertices = CubeVertices;
        break;
    case EPrimitiveType::EPT_Sphere:
        VerticeSize = std::size(SphereVertices);
        Vertices = SphereVertices;
        break;
    case EPrimitiveType::EPT_Cylinder:
    {
		PrimitiveVertices.Add(EPrimitiveType::EPT_Cylinder, CreateCylinderVertices());
		VerticeSize = PrimitiveVertices[EPrimitiveType::EPT_Cylinder].Num();
        Vertices = PrimitiveVertices[EPrimitiveType::EPT_Cylinder].GetData();
        break;
    }
    case EPrimitiveType::EPT_Cone:
    {
        PrimitiveVertices.Add(EPrimitiveType::EPT_Cone, CreateConeVertices());
        VerticeSize = PrimitiveVertices[EPrimitiveType::EPT_Cone].Num();
        Vertices = PrimitiveVertices[EPrimitiveType::EPT_Cone].GetData();
        break;
    }
    }
    VertexBuffer = UEngine::Get().GetRenderer()->CreateVertexBuffer(Vertices, sizeof(FVertexSimple) * VerticeSize);

    return VertexBufferInfo(VertexBuffer, VerticeSize, Topology, Vertices);
}


TArray<FVertexSimple> FBufferCache::CreateConeVertices()
{
    TArray<FVertexSimple> vertices;

    int segments = 36;
    float radius = 1.f;
    float height = 1.f;

	// Cone bottom
    for (int i = 0; i < segments; ++i)
    {
        float angle = 2.0f * PI * i / segments;
        float nextAngle = 2.0f * PI * (i + 1) / segments;

        float x1 = radius * cos(angle);
        float y1 = radius * sin(angle);
        float x2 = radius * cos(nextAngle);
        float y2 = radius * sin(nextAngle);

        // Bottom Triangles(CCW)
        vertices.Add({ 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f });
        vertices.Add({ x2, y2, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f });
        vertices.Add({ x1, y1, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f });

        // Side Triangles(CW)
        vertices.Add({ x1, y1, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f });
        vertices.Add({ x2, y2, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f });
        vertices.Add({ 0.0f, 0.0f, height, 0.0f, 1.0f, 0.0f, 1.0f });
    }

    return vertices;
}

TArray<FVertexSimple> FBufferCache::CreateCylinderVertices()
{
    TArray<FVertexSimple> vertices;
        
    int segments = 36;
    float radius = .03f;
    float height = .5f;


	// Cylinder bottom top
    for (int i = 0; i < segments; ++i)
    {
        float angle = 2.0f * PI * i / segments;
        float nextAngle = 2.0f * PI * (i + 1) / segments;

        float x1 = radius * cos(angle);
        float y1 = radius * sin(angle);
        float x2 = radius * cos(nextAngle);
        float y2 = radius * sin(nextAngle);

        // Bottom Triangles(CCW)
        vertices.Add({ 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f });
        vertices.Add({ x2, y2, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f });
        vertices.Add({ x1, y1, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f });

        // Top Triangles(CCW)
        vertices.Add({ 0.0f, 0.0f, height, 0.0f, 1.0f, 0.0f, 1.0f });
        vertices.Add({ x1, y1, height, 0.0f, 1.0f, 0.0f, 1.0f });
        vertices.Add({ x2, y2, height, 0.0f, 1.0f, 0.0f, 1.0f });

        // Side(Outer) Triangles(CW)
        vertices.Add({ x1, y1, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f });
        vertices.Add({ x2, y2, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f });
        vertices.Add({ x1, y1, height, 0.0f, 0.0f, 1.0f, 1.0f });

        vertices.Add({ x2, y2, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f });
        vertices.Add({ x2, y2, height, 0.0f, 0.0f, 1.0f, 1.0f });
        vertices.Add({ x1, y1, height, 0.0f, 0.0f, 1.0f, 1.0f });
    }

    return vertices;
}
