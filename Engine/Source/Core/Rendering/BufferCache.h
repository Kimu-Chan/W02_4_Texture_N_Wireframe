#pragma once

#define _TCHAR_DEFINED

#include "Core/Container/Array.h"
#include "Core/Container/Map.h"
#include "Primitive/PrimitiveVertices.h"
#include "Core/Math/Box.h"


struct Box;

struct FVertexBufferInfo
{
public:
	FVertexBufferInfo() = default;
	FVertexBufferInfo(ID3D11Buffer* InVertexBuffer, int InVertexBufferSize, D3D_PRIMITIVE_TOPOLOGY InTopology, const FVertexSimple* InVertices)
	{
		VertexBuffer = InVertexBuffer;
		VertexBufferSize = InVertexBufferSize;
		Topology = InTopology;
		SetLocalBounds(InVertices, InVertexBufferSize);
	}

	ID3D11Buffer* GetVertexBuffer() const { return VertexBuffer.Get(); }
	int GetSize() const { return VertexBufferSize; }
	D3D_PRIMITIVE_TOPOLOGY GetTopology() const { return Topology; }
	FVector LocalMin;
	FVector LocalMax;

	void SetLocalBounds(const FVertexSimple* Vertices, UINT Size)
	{
		LocalMin = FVector(FLT_MAX, FLT_MAX, FLT_MAX);
		LocalMax = FVector(-FLT_MAX, -FLT_MAX, -FLT_MAX);

		for (int i = 0; i < Size; ++i)
		{
			LocalMin.X = FMath::Min(LocalMin.X, Vertices[i].X);
			LocalMin.Y = FMath::Min(LocalMin.Y, Vertices[i].Y);
			LocalMin.Z = FMath::Min(LocalMin.Z, Vertices[i].Z);
			LocalMax.X = FMath::Max(LocalMax.X, Vertices[i].X);
			LocalMax.Y = FMath::Max(LocalMax.Y, Vertices[i].Y);
			LocalMax.Z = FMath::Max(LocalMax.Z, Vertices[i].Z);
		}
	}

	FVector GetMin() const { return LocalMin; }
	FVector GetMax() const { return LocalMax; }

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
	D3D_PRIMITIVE_TOPOLOGY Topology;
	int VertexBufferSize;
};

struct FIndexBufferInfo
{
public:
	FIndexBufferInfo() = default;
	FIndexBufferInfo(ID3D11Buffer* InIndexBuffer, int InIndexBufferSize)
	{
		IndexBuffer = InIndexBuffer;
		IndexBufferSize = InIndexBufferSize;
	}
	ID3D11Buffer* GetIndexBuffer() const { return IndexBuffer.Get(); }
	int GetSize() const { return IndexBufferSize; }

	Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuffer;
	int IndexBufferSize;

};

class FBufferCache
{
private:
	TMap <EPrimitiveType, FVertexBufferInfo> VertexBufferCache;
	TMap <EPrimitiveType, FIndexBufferInfo> IndexBufferCache;
	TMap <EPrimitiveType, TArray<FVertexSimple>> PrimitiveVertices;

public:
	FBufferCache();
	~FBufferCache();

	void Init();
	FVertexBufferInfo GetVertexBufferInfo(EPrimitiveType Type);
	FIndexBufferInfo GetIndexBufferInfo(EPrimitiveType Type);

public:
	TArray<FVertexSimple> CreateConeVertices();
	TArray<FVertexSimple> CreateCylinderVertices();

private :
	FVertexBufferInfo CreateVertexBufferInfo(EPrimitiveType Type);
	FIndexBufferInfo CreateIndexBufferInfo(EPrimitiveType Type);
	TArray<UINT> CreateDefaultIndices(int Size);
};

