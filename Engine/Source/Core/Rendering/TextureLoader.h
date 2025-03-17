#pragma once

#define RESOURCE_PATH L"Resources/"


struct TextureInfo
{
	ID3D11ShaderResourceView* ShaderResourceView;
	int32 Rows = 1; // 아틀라스 이미지 행 개수
	int32 Cols = 1; // 아틀라스 이미지 열 개수
};

class TextureLoader
{
public:
	TextureLoader(ID3D11Device* InDevice, ID3D11DeviceContext* InContext);
	~TextureLoader();

	bool LoadTexture(const std::wstring& Name, const std::wstring& FileName, int32 InRows, int32 InColumns);
	const TextureInfo* GetTextureInfo(const std::wstring& Name) const;
	void ReleaseTextures();

private:
	ID3D11Device* Device;
	ID3D11DeviceContext* Context;
	std::unordered_map<std::wstring, TextureInfo> TextureMap;

	std::wstring GetFullPath(const std::wstring& FileName) const;
};

