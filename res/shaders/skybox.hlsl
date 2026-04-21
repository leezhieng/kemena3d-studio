// NOTE: Assumes row-major matrices (standard HLSL). If uploading OpenGL column-major
//       matrices, either transpose before upload or declare float4x4 as column_major.

// =============================================================================
// VERTEX SHADER
// =============================================================================

cbuffer PerFrame : register(b0)
{
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

struct VSInput
{
    float3 position : POSITION;
};

struct VSOutput
{
    float4 position : SV_Position;
    float3 texCoord : TEXCOORD0;
};

VSOutput VSMain(VSInput input)
{
    VSOutput o;
    o.texCoord = input.position;
    float4 pos = mul(projectionMatrix, mul(viewMatrix, float4(input.position, 1.0)));
    // Force depth to far plane (z = w → NDC depth = 1.0)
    o.position = pos.xyww;
    return o;
}

// =============================================================================
// PIXEL SHADER
// =============================================================================

TextureCube  cubeMap        : register(t0);
SamplerState defaultSampler : register(s0);

float4 PSMain(VSOutput input) : SV_Target
{
    return cubeMap.Sample(defaultSampler, input.texCoord);
}
