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

	bool LoadTexture(const FName& Name, const FString& FileName, int32 InRows, int32 InColumns);
	TextureInfo* GetTextureInfo(const FName& Name);
	void ReleaseTextures();

private:
	ID3D11Device* Device;
	ID3D11DeviceContext* Context;
	std::unordered_map<FName, TextureInfo> TextureMap;

	FString GetFullPath(const FString& FileName) const;
};

