
Texture2D TextureMap : register(t0);

SamplerState SampleType : register(s0);

cbuffer TextureBuffer : register(b5)
{
    matrix WorldViewProj;
    float2 UVOffset;
    float2 AtlasColsRows;
    int bIsText; // 0 이면 일반 텍스처, 1 이면 텍스트
}

struct VS_INPUT
{
    float3 Position : POSITION;
    float2 Tex : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 Tex : TEXCOORD0;
};

PS_INPUT mainVS(VS_INPUT Input)
{
    PS_INPUT Output;
    Output.Position = mul(float4(Input.Position.xyz, 1.0f), WorldViewProj);
    
    if (1 == bIsText)
    {
        Output.Tex = Input.Tex;
    }
    else
    {
        float2 AtlasTileSize = float2(1.0f / AtlasColsRows.x, 1.0f / AtlasColsRows.y);
        Output.Tex = Input.Tex * AtlasTileSize + UVOffset;
    }
    
    return Output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    float4 Color = TextureMap.Sample(SampleType, input.Tex);
    
    float Threshold = 0.1f;
    
    if (Color.r < Threshold && Color.g < Threshold && Color.b < Threshold)
    {
        Color.a = 0.f;
    }
    else
    {
        Color.a = 1.f;
    }
    
    return pow(Color, 2);
}
