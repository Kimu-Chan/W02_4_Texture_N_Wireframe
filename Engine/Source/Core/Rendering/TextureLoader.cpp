#include "pch.h"
#include "TextureLoader.h"
#include "CoreUObject/NameTypes.h"
#include <DirectXTK/WICTextureLoader.h>

TextureLoader::TextureLoader(ID3D11Device* InDevice, ID3D11DeviceContext* InContext)
	: Device(InDevice), Context(InContext)
{
}

TextureLoader::~TextureLoader()
{
    ReleaseTextures();
}

bool TextureLoader::LoadTexture(const FName& Name, const FString& FileName, int32 InRows, int32 InColumns)
{
    // 맵 확인
    TextureInfo* Info = TextureMap.Find(Name);

    if (Info)
    {
        return true;
    }

    FString FullPath = GetFullPath(FileName);

    // DirectX 텍스처 로드
    ID3D11ShaderResourceView* ShaderResourceView;
    HRESULT Result = DirectX::CreateWICTextureFromFile(Device, Context, FullPath.c_wchar(), nullptr, &ShaderResourceView);
	if (FAILED(Result))
	{
		return false;
	}
    
    // 텍스처 정보 추가
    TextureInfo NewInfo;
    NewInfo.Rows = InRows;
    NewInfo.Cols = InColumns;
    NewInfo.ShaderResourceView = ShaderResourceView;
    
	TextureMap.Add(Name, NewInfo);

    return true;
}

TextureInfo* TextureLoader::GetTextureInfo(const FName& Name)
{
    TextureInfo* Info = TextureMap.Find(Name);
    return Info;
}

void TextureLoader::ReleaseTextures()
{
	for (auto& Pair : TextureMap)
	{
		Pair.Value.ShaderResourceView->Release();
	}
    TextureMap.Empty();
}

FString TextureLoader::GetFullPath(const FString& FileName) const
{
    return RESOURCE_PATH + FileName;
}
