Texture2D TextureMap : register(t0);
SamplerState SampleType : register(s0);

cbuffer TextureBuffer : register(b5)
{
    matrix WorldViewProj;
    float2 UVOffset;
}

struct VS_INPUT
{
    float4 Position : POSITION;
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
    Output.Position = mul(Input.Position, WorldViewProj);
    Output.Tex = Input.Tex + UVOffset;
    return Output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    return TextureMap.Sample(SampleType, input.Tex);
}
