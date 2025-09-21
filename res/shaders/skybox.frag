#version 330 core

in vec3 texCoordFrag;

out vec4 fragColor;

uniform samplerCube cubeMap;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
uniform Material material;

void main()
{
    fragColor = texture(cubeMap, texCoordFrag);
}