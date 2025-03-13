#pragma once

#define _TCHAR_DEFINED

#include "Core/Container/Array.h"
#include "Core/Container/Map.h"
#include "Primitive/PrimitiveVertices.h"
#include "Core/Math/Box.h"


struct Box;

struct BufferInfo
{
public:
	BufferInfo() = default;
	BufferInfo(ID3D11Buffer* InBuffer, int BufferSize, D3D_PRIMITIVE_TOPOLOGY InTopology, const FVertexSimple* InVertices)
	{
		Buffer = InBuffer;
		Size = BufferSize;
		Topology = InTopology;
		SetLocalBounds(InVertices, BufferSize);
	}

	ID3D11Buffer* GetBuffer() const { return Buffer.Get(); }
	int GetSize() const { return Size; }
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
	Microsoft::WRL::ComPtr<ID3D11Buffer> Buffer;
	D3D_PRIMITIVE_TOPOLOGY Topology;
	int Size;
};

class FBufferCache
{
private:
	TMap <EPrimitiveType, BufferInfo> Cache;

public:
	FBufferCache();
	~FBufferCache();

	void Init();
	BufferInfo GetBufferInfo(EPrimitiveType Type);

public:
	TArray<FVertexSimple> CreateConeVertices();
	TArray<FVertexSimple> CreateCylinderVertices();

private :
	BufferInfo CreateVertexBufferInfo(EPrimitiveType Type);
};

