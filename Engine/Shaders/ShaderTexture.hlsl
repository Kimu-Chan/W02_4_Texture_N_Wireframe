Texture2D TextureMap : register(t0);
SamplerState SampleType : register(s0);

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 Tex : TEXCOORD;
};

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    return TextureMap.Sample(SampleType, input.Tex);
}
