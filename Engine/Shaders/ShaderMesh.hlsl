// ShaderMain.hlsl

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
}

cbuffer ChangeOnResizeAndFov : register(b2)
{
    matrix ProjectionMatrix;
    float NearClip;
    float FarClip;
}

cbuffer UUIDColor : register(b3)
{
    float4 UUIDColor;
}

////////
/// Input, Output
////////
struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION; // Transformed position to pass to the pixel shader
    float4 Color : COLOR;          // Color to pass to the pixel shader
};

////////
/// Function
////////
PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    output.Position = float4(input.Position.xyz, 1.0f);
    output.Position = mul(output.Position, WorldMatrix);
    output.Position = mul(output.Position, ViewMatrix);
    output.Position = mul(output.Position, ProjectionMatrix);

    output.Color = CustomColor;
    return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    return input.Color;
}

float4 PickingPS(PS_INPUT input) : SV_TARGET
{
    return UUIDColor;
}
