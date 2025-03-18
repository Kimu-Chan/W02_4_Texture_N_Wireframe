#pragma once
#include "Container/Array.h"
#include "Container/String.h"
#include "Math/Vector.h"
#include "Rendering/RendererDefine.h"

class MeshBuilder
{
public:
    MeshBuilder();

    ~MeshBuilder();

    bool BuildMeshFromObj(const FString& ObjPath);

    uint32 GetVertexNum() const { return Vertices.Num(); }
    uint32 GetIndexNum() const { return Indices.Num(); }

    TArray<FStaticMeshVertex> GetVertices() { return Vertices; }
    TArray<uint32> GetIndices() { return Indices; }

private:
    void MakeVertex(const TArray<float>& Vertex, const TArray<float>& Normal, const TArray<float>& UV, FStaticMeshVertex& OutVertex);

    void CalculateTangent(const FStaticMeshVertex& Vertex0, const FStaticMeshVertex& Vertex1, const FStaticMeshVertex& Vertex2, FVector& OutTangent);
    
    TArray<FStaticMeshVertex> Vertices;
    TArray<uint32> Indices;

    uint32 VerticesNum;
    uint32 IndicesNum;
};
