#pragma once

#define _TCHAR_DEFINED

#include "NameTypes.h"
#include "Core/Container/Array.h"
#include "Core/Container/Map.h"
#include "Primitive/PrimitiveVertices.h"
#include "Core/Math/Box.h"


struct Box;

struct FVertexBufferInfo
{
public:
	FVertexBufferInfo() = default;
	FVertexBufferInfo(ID3D11Buffer* InVertexBuffer, uint32 InVertexBufferSize, D3D_PRIMITIVE_TOPOLOGY InTopology, const FVertexSimple* InVertices)
	{
		VertexBuffer = InVertexBuffer;
		VertexBufferSize = InVertexBufferSize;
		Topology = InTopology;
		SetLocalBounds(InVertices, InVertexBufferSize);
	}

	ID3D11Buffer* GetVertexBuffer() const { return VertexBuffer.Get(); }
	uint32 GetSize() const { return VertexBufferSize; }
	D3D_PRIMITIVE_TOPOLOGY GetTopology() const { return Topology; }
	FVector LocalMin;
	FVector LocalMax;

	void SetLocalBounds(const FVertexSimple* Vertices, UINT Size)
	{
		if (Vertices == nullptr)
		{
			return;
		}
		
		LocalMin = FVector(FLT_MAX, FLT_MAX, FLT_MAX);
		LocalMax = FVector(-FLT_MAX, -FLT_MAX, -FLT_MAX);

		for (uint32 i = 0; i < Size; ++i)
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
	uint32 VertexBufferSize;
};

struct FIndexBufferInfo
{
public:
	FIndexBufferInfo() = default;
	FIndexBufferInfo(ID3D11Buffer* InIndexBuffer, uint32 InIndexBufferSize)
	{
		IndexBuffer = InIndexBuffer;
		IndexBufferSize = InIndexBufferSize;
	}
	ID3D11Buffer* GetIndexBuffer() const { return IndexBuffer.Get(); }
	uint32 GetSize() const { return IndexBufferSize; }

	Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuffer;
	uint32 IndexBufferSize;

};

struct FStaticMeshBufferInfo
{
public:
	FStaticMeshBufferInfo() = default;
	FStaticMeshBufferInfo(const FVertexBufferInfo& InVertexBufferInfo, const FIndexBufferInfo& InIndexBufferInfo)
	{
		VertexBufferInfo = InVertexBufferInfo;
		IndexBufferInfo = InIndexBufferInfo;
	}

	FVertexBufferInfo VertexBufferInfo;
	FIndexBufferInfo IndexBufferInfo;
};

class FBufferCache
{
private:
	TMap <EPrimitiveType, FVertexBufferInfo> VertexBufferCache;
	TMap <EPrimitiveType, FIndexBufferInfo> IndexBufferCache;
	TMap <EPrimitiveType, TArray<FVertexSimple>> PrimitiveVertices;

	TMap<FName, FStaticMeshBufferInfo> StaticMeshBufferCache;

public:
	FBufferCache();
	~FBufferCache();

	void Init();
	
	FVertexBufferInfo GetVertexBufferInfo(EPrimitiveType Type);
	FIndexBufferInfo GetIndexBufferInfo(EPrimitiveType Type);

	FStaticMeshBufferInfo GetStaticMeshBufferInfo(FName InName);
	bool BuildStaticMesh(const FString& ObjFilePath);

public:
	TArray<FVertexSimple> CreateConeVertices();
	TArray<FVertexSimple> CreateCylinderVertices();

private :
	FVertexBufferInfo CreateVertexBufferInfo(EPrimitiveType Type);
	FIndexBufferInfo CreateIndexBufferInfo(EPrimitiveType Type);
	TArray<UINT> CreateDefaultIndices(int Size);
};

