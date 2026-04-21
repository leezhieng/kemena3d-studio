// NOTE: Assumes row-major matrices (standard HLSL). If uploading OpenGL column-major
//       matrices, either transpose before upload or declare float4x4 as column_major.
//
// NOTE: linearizeDepth uses SV_Position.z which is [0,1] in D3D (vs OpenGL's gl_FragCoord.z
//       which is also [0,1] but mapped from a different NDC range). The formula produces
//       a slightly different fade curve than the GLSL version — adjust if needed.

// =============================================================================
// VERTEX SHADER
// =============================================================================

static const float GRID_SIZE = 100.0;

cbuffer PerFrame : register(b0)
{
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float3   viewPos;
    float    _pad;
};

struct VSInput
{
    float3 position : POSITION;
};

struct VSOutput
{
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD0;
};

VSOutput VSMain(VSInput input)
{
    float3 wp  = input.position * GRID_SIZE;
    wp.x      += viewPos.x;
    wp.z      += viewPos.z;

    VSOutput o;
    o.texCoord = wp.xz;
    o.position = mul(projectionMatrix, mul(viewMatrix, float4(wp, 1.0)));
    return o;
}

// =============================================================================
// PIXEL SHADER
// =============================================================================

struct PSInput
{
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD0;
};

float linearizeDepth(float d, float zNear, float zFar)
{
    return (2.0 * zNear) / (zFar + zNear - d * (zFar - zNear));
}

float4 PSMain(PSInput input) : SV_Target
{
    float zbuf = linearizeDepth(input.position.z, 2.0, 4500.0);

    const float cellSize    = 1.0;
    const float subcellSize = 1.0;
    const float halfCell    = cellSize    * 0.5;
    const float halfSub     = subcellSize * 0.5;

    float2 cell    = fmod(input.texCoord / 50.0 + halfCell + 0.5,    cellSize);
    float2 subcell = fmod(input.texCoord / 10.0 + halfSub  + 0.5, subcellSize);

    float4 color = float4(0, 0, 0, 0);

    if      (frac(cell.x    / 0.1) < 0.01 || frac(cell.y    / 0.1) < 0.01)
        color = float4(0.75, 0.75, 0.75, 0.8);
    else if (frac(subcell.x / 0.1) < 0.02 || frac(subcell.y / 0.1) < 0.02)
        color = float4(0.5,  0.5,  0.5,  0.8);

    color.a = max(color.a - zbuf, 0.0);
    return color;
}
