#include "pch.h"
#include "TextureLoader.h"
#include <DirectXTK/WICTextureLoader.h>

TextureLoader::TextureLoader(ID3D11Device* InDevice, ID3D11DeviceContext* InContext)
	: Device(InDevice), Context(InContext)
{
}

TextureLoader::~TextureLoader()
{
    ReleaseTextures();
}

bool TextureLoader::LoadTexture(const std::wstring& Name, const std::wstring& FileName, int32 InRows, int32 InColumns)
{
    // 맵 확인
    std::unordered_map<std::wstring, TextureInfo>::iterator iter = TextureMap.find(Name);
    if (iter != TextureMap.end())
    {
        return true;
    }

    std::wstring FullPath = GetFullPath(FileName);

    // DirectX 텍스처 로드
    ID3D11ShaderResourceView* ShaderResourceView;
    HRESULT Result = DirectX::CreateWICTextureFromFile(Device, Context, FullPath.c_str() , nullptr, &ShaderResourceView);
	if (FAILED(Result))
	{
		return false;
	}
    
    // 텍스처 정보 추가
    TextureInfo Info;
    Info.Rows = InRows;
    Info.Cols = InColumns;
	Info.ShaderResourceView = ShaderResourceView;
    
    TextureMap[Name] = Info;

    return true;
}

TextureInfo* TextureLoader::GetTextureInfo(const std::wstring& Name) 
{
    std::unordered_map<std::wstring, TextureInfo>::iterator iter = TextureMap.find(Name);
    if (TextureMap.end() != iter)
    {
        return &iter->second;
    }
    return nullptr;
}

void TextureLoader::ReleaseTextures()
{
	for (auto& Pair : TextureMap)
	{
		Pair.second.ShaderResourceView->Release();
	}
	TextureMap.clear();
}

std::wstring TextureLoader::GetFullPath(const std::wstring& FileName) const
{
    return RESOURCE_PATH + FileName;
}
