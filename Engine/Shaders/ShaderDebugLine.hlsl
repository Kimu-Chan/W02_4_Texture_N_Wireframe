// ShaderDebugLine.hlsl

////////
/// Constant Buffer
////////
cbuffer ChangeEveryFrame : register(b1)
{
    matrix ViewMatrix;
    float3 ViewPosition;
}

cbuffer ChangeOnResizeAndFov : register(b2)
{
    matrix ProjectionMatrix;
    float NearClip;
    float FarClip;
}

////////
/// Input, Output
////////
struct VS_INPUT
{
    float3 Position : POSITION;
    float4 Color : COLOR;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

////////
/// Function
////////
PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    
    output.Position = float4(input.Position.xyz, 1.0f);
    output.Position = mul(output.Position, ViewMatrix);
    output.Position = mul(output.Position, ProjectionMatrix);

    output.Color = input.Color;
    return output;
}


float4 mainPS(PS_INPUT input) : SV_TARGET
{
    return input.Color;
}