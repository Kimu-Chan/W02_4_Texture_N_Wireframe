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
    float4 position : POSITION; // Input position from vertex buffer
};

struct PS_INPUT
{
    float4 position : SV_POSITION; // Transformed position to pass to the pixel shader
    float4 color : COLOR;          // Color to pass to the pixel shader
    // float4 depthPosition : TEXCOORD0;
};

struct PS_OUTPUT
{
    float4 color : SV_TARGET;
    float depth : SV_Depth;
};

////////
/// Function
////////
PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    output.position = input.position;
    output.position = mul(output.position, WorldMatrix);
    output.position = mul(output.position, ViewMatrix);
    output.position = mul(output.position, ProjectionMatrix);

    output.color = CustomColor;
    return output;
}


float4 mainPS(PS_INPUT input) : SV_TARGET
{
    return input.color;
}