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
    ComPtr<ID3D11ShaderResourceView> ShaderResourceView;
    HRESULT Result = DirectX::CreateWICTextureFromFile(Device, Context, FullPath.c_str() , nullptr, ShaderResourceView.GetAddressOf());
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

const TextureInfo* TextureLoader::GetTextureInfo(const std::wstring& Name) const
{
    std::unordered_map<std::wstring, TextureInfo>::const_iterator iter = TextureMap.find(Name);
    if (TextureMap.end() != iter)
    {
        return &iter->second;
    }
    return nullptr;
}

void TextureLoader::ReleaseTextures()
{
	TextureMap.clear();
}

std::wstring TextureLoader::GetFullPath(const std::wstring& FileName) const
{
    return RESOURCE_PATH + FileName;
}
