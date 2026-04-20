#version 330 core

in vec2 texCoordFrag;
out vec4 fragColor;

uniform sampler2D albedoMap;
uniform bool has_albedoMap;

struct Material {
	vec2 tiling;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
	float metallic;
	float roughness;
};
uniform Material material;

void main()
{
	vec4 albedo = has_albedoMap ? texture(albedoMap, texCoordFrag * material.tiling) : vec4(1.0);
	fragColor = vec4(material.diffuse, 1.0) * albedo;
}
