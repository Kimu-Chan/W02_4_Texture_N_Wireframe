#pragma once
#include "Core/HAL/PlatformType.h"
#include "Core/Container/Map.h"
#include "Core/Container/String.h"
#include "Core/Container/Array.h"

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
	TArray<std::wstring> GetShaderNames(const std::wstring& Directory) const;
	void CreateShaders(const TArray<std::wstring>& ShaderNames);

	ID3D11VertexShader* GetVertexShader(const std::wstring& ShaderName) const;
	ID3D11PixelShader* GetPixelShader(const std::wstring& ShaderName) const;
	ID3D11InputLayout* GetInputLayout(const std::wstring& ShaderName) const;

private:
	bool CompileShader(const std::wstring& ShaderPath, const std::string& EntryPoint, const std::string& ShaderModel, ID3DBlob** ShaderBlob);
	bool CreateInputLayout(ID3DBlob* VertexShaderBlob, ID3D11InputLayout** InputLayout);

private:
	TMap<std::wstring, FShaderData> ShaderDatas;
};

