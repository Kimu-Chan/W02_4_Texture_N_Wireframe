#pragma once
#include "Core/HAL/PlatformType.h"
#include "Core/Container/Map.h"
#include "Core/Container/String.h"
#include "Core/Container/Array.h"
#include "NameTypes.h"

struct FShaderData
{
	ComPtr<ID3D11VertexShader> VertexShader;
	ComPtr<ID3D11PixelShader> PixelShader;
	ComPtr<ID3D11InputLayout> InputLayout;
};

struct FShaderCache
{
public:
	FShaderCache() = default;
	~FShaderCache() = default;

public:
	TArray<FString> GetShaderNames(const FString& Directory) const;
	void CreateShaders(const TArray<FString>& ShaderNames);

	ID3D11VertexShader* GetVertexShader(const FString& ShaderName) const;
	ID3D11PixelShader* GetPixelShader(const FString& ShaderName) const;
	ID3D11InputLayout* GetInputLayout(const FString& ShaderName) const;

private:
	bool CompileShader(const FString& ShaderPath, const FString& EntryPoint, const FString& ShaderModel, ID3DBlob** ShaderBlob);
	bool CreateInputLayout(ID3DBlob* VertexShaderBlob, ID3D11InputLayout** InputLayout);

private:
	TMap<FName, FShaderData> ShaderDatas;
};

