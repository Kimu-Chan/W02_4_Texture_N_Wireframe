#include "pch.h" 
#include "BufferCache.h"
#include "Engine/Engine.h"
#include "Core/Container/Array.h"
#include "Primitive/PrimitiveVertices.h"

FBufferCache::FBufferCache() {}

FBufferCache::~FBufferCache() {}

void FBufferCache::Init() {}

BufferInfo FBufferCache::GetBufferInfo(EPrimitiveType Type)
{
    if (!Cache.contains(Type))
    {
        auto bufferInfo = CreateVertexBufferInfo(Type);
        Cache.insert({ Type, bufferInfo });
    }

    return Cache[Type];
}

BufferInfo FBufferCache::CreateVertexBufferInfo(EPrimitiveType Type)
{
    ID3D11Buffer* Buffer = nullptr;
    int Size = 0;
    D3D_PRIMITIVE_TOPOLOGY Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    switch (Type)
    {
    case EPrimitiveType::EPT_Line:
        Size = std::size(LineVertices);
        Buffer = UEngine::Get().GetRenderer()->CreateVertexBuffer(LineVertices, sizeof(FVertexSimple) * Size);
        Topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
        break;
    case EPrimitiveType::EPT_Triangle:
        Size = std::size(TriangleVertices);
        Buffer = UEngine::Get().GetRenderer()->CreateVertexBuffer(TriangleVertices, sizeof(FVertexSimple) * Size);
        break;
    case EPrimitiveType::EPT_Cube:
        Size = std::size(CubeVertices);
        Buffer = UEngine::Get().GetRenderer()->CreateVertexBuffer(CubeVertices, sizeof(FVertexSimple) * Size);
        break;
    case EPrimitiveType::EPT_Sphere:
        Size = std::size(SphereVertices);
        Buffer = UEngine::Get().GetRenderer()->CreateVertexBuffer(SphereVertices, sizeof(FVertexSimple) * Size);
        break;
    case EPrimitiveType::EPT_Cylinder:
        {
            /**
             * case문 특성상 다른 case에서도 Vertices 변수에 접근 가능.
             * 하지만 초기화는 이 case에서만 하고있으므로, 다른 case에서 Vertices에 접근하면 초기화 되지 않은 변수에 접근하는 문제 발생.
             * 따라서 Vertices 변수의 스코프를 이 case로 제한하여 다른 case에서 접근 불가능하도록 제한. 
             */
            TArray<FVertexSimple> Vertices = CreateCylinderVertices();
            Size = Vertices.Num();
            Buffer = UEngine::Get().GetRenderer()->CreateVertexBuffer(Vertices.GetData(), sizeof(FVertexSimple) * Size);
            break;
        }
    case EPrimitiveType::EPT_Cone:
        {
            TArray<FVertexSimple> Vertices = CreateConeVertices();
            Size = Vertices.Num();
            Buffer = UEngine::Get().GetRenderer()->CreateVertexBuffer(Vertices.GetData(), sizeof(FVertexSimple) * Size);
            break;
        }
    }

    return BufferInfo(Buffer, Size, Topology);
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
