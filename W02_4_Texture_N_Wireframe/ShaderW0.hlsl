// ShaderW0.hlsl
cbuffer constants : register(b0)
{
    row_major float4x4 MVP;
    float Flag;
}


struct VS_INPUT
{
    float4 position : POSITION; // Input position from vertex buffer
    float4 color : COLOR; // Input color from vertex buffer
};

struct PS_INPUT
{
    float4 position : SV_POSITION; // Transformed position to pass to the pixel shader
    float4 color : COLOR; // Color to pass to the pixel shader
};


PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output = input;
    
    float4x4 modelViewProjection = MVP;  // MVP 행렬을 그대로 사용
    output.position = mul(float4(input.position.xyz, 1.0f), modelViewProjection);

    // Pass the color to the pixel shader
    
    output.color = input.color;
    if(Flag == 1.0f){
        output.color.rgb *= 1.5f;
    }
    return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    // Output the color directly
    return input.color;
}