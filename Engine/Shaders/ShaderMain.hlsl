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
    float3 position : POSITION; // Input position from vertex buffer
    float4 color : COLOR;       // Input color from vertex buffer
};

struct PS_INPUT
{
    float4 position : SV_POSITION; // Transformed position to pass to the pixel shader
    float4 color : COLOR;          // Color to pass to the pixel shader
};

////////
/// Function
////////
PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    output.position = float4(input.position.xyz, 1.0f);
    output.position = mul(output.position, WorldMatrix);
    output.position = mul(output.position, ViewMatrix);
    output.position = mul(output.position, ProjectionMatrix);

    output.color = bUseVertexColor == 1 ? input.color : CustomColor;
    return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    return input.color;
}

float4 PickingPS(PS_INPUT input) : SV_TARGET
{
    return UUIDColor;
}