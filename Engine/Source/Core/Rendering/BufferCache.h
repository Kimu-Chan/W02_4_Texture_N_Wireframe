#pragma once

#define _TCHAR_DEFINED

#include "Core/Container/Array.h"
#include "Primitive/PrimitiveVertices.h"

struct BufferInfo
{
public:
	BufferInfo() = default;
	BufferInfo(ID3D11Buffer* InBuffer, int BufferSize, D3D_PRIMITIVE_TOPOLOGY InTopology)
	{
		Buffer = InBuffer;
		Size = BufferSize;
		Topology = InTopology;
	}

	ID3D11Buffer* GetBuffer() const { return Buffer.Get(); }
	int GetSize() const { return Size; }
	D3D_PRIMITIVE_TOPOLOGY GetTopology() const { return Topology; }

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> Buffer;
	D3D_PRIMITIVE_TOPOLOGY Topology;
	int Size;
};

class FBufferCache
{
private:
	std::unordered_map <EPrimitiveType, BufferInfo> Cache;

public:
	FBufferCache();
	~FBufferCache();

	void Init();
	BufferInfo GetBufferInfo(EPrimitiveType Type);

public:
	// 동적으로 버텍스 정보 생성
	TArray<FVertexSimple> CreateArrowVertices();
	TArray<FVertexSimple> CreateConeVertices();
	TArray<FVertexSimple> CreateCylinderVertices();

private :
	BufferInfo CreateVertexBufferInfo(EPrimitiveType Type);
};

