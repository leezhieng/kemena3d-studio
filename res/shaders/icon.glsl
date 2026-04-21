#version 330 core

uniform vec3 cameraRightWorldSpace;
uniform vec3 cameraUpWorldSpace;
uniform mat4 viewProjection;
uniform vec3 billboardPosition;
uniform vec2 billboardSize;

layout(location = 0) in vec3 vertexPosition;

out vec2 v_uv;

void main()
{
    vec3 worldPos = billboardPosition
        + cameraRightWorldSpace * vertexPosition.x * billboardSize.x
        + cameraUpWorldSpace    * vertexPosition.y * billboardSize.y;

    v_uv        = vertexPosition.xy + vec2(0.5, 0.5);
    gl_Position = viewProjection * vec4(worldPos, 1.0);
}

// --- FRAGMENT ---

#version 330 core

uniform sampler2D albedoMap;
uniform vec3      color;

const float alphaCutoff = 0.3;

in  vec2 v_uv;
out vec4 fragColor;

void main()
{
    vec4 tex = texture(albedoMap, v_uv);
    if (tex.a < alphaCutoff) discard;
    fragColor = tex * vec4(color, 1.0);
}
