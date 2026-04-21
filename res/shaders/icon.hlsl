// NOTE: Assumes row-major matrices (standard HLSL). If uploading OpenGL column-major
//       matrices, either transpose before upload or declare float4x4 as column_major.

// =============================================================================
// VERTEX SHADER
// =============================================================================

cbuffer PerBillboard : register(b0)
{
    float3   cameraRightWorldSpace;
    float    _pad0;
    float3   cameraUpWorldSpace;
    float    _pad1;
    float4x4 viewProjection;
    float3   billboardPosition;
    float    _pad2;
    float2   billboardSize;
    float2   _pad3;
};

struct VSInput
{
    float3 position : POSITION;
};

struct VSOutput
{
    float4 position : SV_Position;
    float2 uv       : TEXCOORD0;
};

VSOutput VSMain(VSInput input)
{
    float3 worldPos = billboardPosition
        + cameraRightWorldSpace * input.position.x * billboardSize.x
        + cameraUpWorldSpace    * input.position.y * billboardSize.y;

    VSOutput o;
    o.uv       = input.position.xy + float2(0.5, 0.5);
    o.position = mul(viewProjection, float4(worldPos, 1.0));
    return o;
}

// =============================================================================
// PIXEL SHADER
// =============================================================================

static const float ALPHA_CUTOFF = 0.3;

cbuffer PerIcon : register(b1)
{
    float3 color;
    float  _pad;
};

Texture2D    albedoMap      : register(t0);
SamplerState defaultSampler : register(s0);

float4 PSMain(VSOutput input) : SV_Target
{
    float4 tex = albedoMap.Sample(defaultSampler, input.uv);
    clip(tex.a - ALPHA_CUTOFF);
    return tex * float4(color, 1.0);
}
