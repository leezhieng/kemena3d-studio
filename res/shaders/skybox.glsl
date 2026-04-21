#version 330 core

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

layout(location = 0) in vec3 vertexPosition;

out vec3 v_texCoord;

void main()
{
    v_texCoord  = vertexPosition;
    vec4 pos    = projectionMatrix * viewMatrix * vec4(vertexPosition, 1.0);
    gl_Position = pos.xyww;
}

// --- FRAGMENT ---

#version 330 core

uniform samplerCube cubeMap;

in  vec3 v_texCoord;
out vec4 fragColor;

void main()
{
    fragColor = texture(cubeMap, v_texCoord);
}
