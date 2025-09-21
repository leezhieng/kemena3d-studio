#version 330 core

in vec2 UV;

uniform sampler2D albedoMap;
uniform vec3 color;

uniform float alphaCutoff = 0.3;

out vec4 fragColor;

void main(){

	vec4 diffuseTexture = texture(albedoMap, UV);

	if (diffuseTexture.a < alphaCutoff)
		discard;

	fragColor = diffuseTexture * vec4(color, 1.0);
}