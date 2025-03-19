// ShaderGrid.hlsl

////////
/// Constant Buffer
////////
cbuffer ChangeEveryObject : register(b0)
{
    matrix WorldMatrix;
    float4 CustomColor;
    uint bUseVertexColor;
}

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
    float3 Position : POSITION; // Input position from vertex buffer
};

struct PS_INPUT
{
    float4 Position : SV_POSITION; // Transformed position to pass to the pixel shader
    float4 Color : COLOR;          // Color to pass to the pixel shader
    float4 WorldPosition : POSITION;
};

////////
/// Function
////////
PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    output.Position = float4(input.Position.xyz, 1.0f);
    output.Position = mul(output.Position, WorldMatrix);
    output.WorldPosition = output.Position;
    output.Position = mul(output.Position, ViewMatrix);
    output.Position = mul(output.Position, ProjectionMatrix);

    output.Color = CustomColor;
    return output;
}


float4 mainPS(PS_INPUT input) : SV_TARGET
{
    float Dist = length(input.WorldPosition.xyz - ViewPosition);

    float MaxDist = FarClip * 1.2f;
    float MinDist = MaxDist * 0.5f;

    // Fade out grid
    float Fade = saturate(1.f - (Dist - MinDist) / (MaxDist - MinDist));
    input.Color.a *= Fade * Fade * Fade;
    
    return input.Color;
}