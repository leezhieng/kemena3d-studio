#version 330 core

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 viewPos;

const float grid_size = 100.0;

layout(location = 0) in vec3 vertexPosition;

out vec2 v_texCoord;

void main()
{
    vec3 wp  = vertexPosition * grid_size;
    wp.x    += viewPos.x;
    wp.z    += viewPos.z;

    v_texCoord  = wp.xz;
    gl_Position = projectionMatrix * viewMatrix * vec4(wp, 1.0);
}

// --- FRAGMENT ---

#version 330 core

in  vec2 v_texCoord;
out vec4 fragColor;

float linearizeDepth(float d, float zNear, float zFar)
{
    return (2.0 * zNear) / (zFar + zNear - d * (zFar - zNear));
}

void main()
{
    float zbuf = linearizeDepth(gl_FragCoord.z, 2.0, 4500.0);

    const float cellSize    = 1.0;
    const float subcellSize = 1.0;
    const float halfCell    = cellSize    * 0.5;
    const float halfSub     = subcellSize * 0.5;

    vec2 cell    = mod(v_texCoord / 50.0 + halfCell + 0.5,    cellSize);
    vec2 subcell = mod(v_texCoord / 10.0 + halfSub  + 0.5, subcellSize);

    vec4 color = vec4(0.0);

    if      (fract(cell.x    / 0.1) < 0.01 || fract(cell.y    / 0.1) < 0.01)
        color = vec4(0.75, 0.75, 0.75, 0.8);
    else if (fract(subcell.x / 0.1) < 0.02 || fract(subcell.y / 0.1) < 0.02)
        color = vec4(0.5,  0.5,  0.5,  0.8);

    color.a   = max(color.a - zbuf, 0.0);
    fragColor = color;
}
