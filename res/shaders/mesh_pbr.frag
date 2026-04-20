#version 330 core

in vec3 vertexPositionFrag;
in vec3 vertexColorFrag;
in vec2 texCoordFrag;
in vec3 vertexNormalFrag;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 viewPos;

in vec3 T;
in vec3 B;
in vec3 N;

out vec4 fragColor;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicRoughnessMap;
uniform sampler2D aoMap;
uniform sampler2D emissiveMap;

uniform bool has_albedoMap;
uniform bool has_normalMap;
uniform bool has_metallicRoughnessMap;
uniform bool has_aoMap;
uniform bool has_emissiveMap;

struct Material {
	vec2  tiling;
	vec3  ambient;
	vec3  diffuse;
	vec3  specular;
	float shininess;
	float metallic;
	float roughness;
};
uniform Material material;

uniform vec3 sceneAmbient;
uniform samplerCube skyboxMap;
uniform bool  skyboxAmbientEnabled;
uniform float skyboxAmbientStrength;

struct SunLight {
	float power;
	vec3  direction;
	vec3  diffuse;
	vec3  specular;
};
uniform int sunLightNum;
uniform SunLight sunLights[32];

struct PointLight {
	float power;
	vec3  position;
	float constant;
	float linear;
	float quadratic;
	vec3  diffuse;
	vec3  specular;
};
uniform int pointLightNum;
uniform PointLight pointLights[32];

struct SpotLight {
	float power;
	vec3  position;
	vec3  direction;
	float cutOff;
	float outerCutOff;
	float constant;
	float linear;
	float quadratic;
	vec3  diffuse;
	vec3  specular;
};
uniform int spotLightNum;
uniform SpotLight spotLights[32];

const float PI = 3.14159265359;

float DistributionGGX(vec3 n, vec3 h, float roughness)
{
	float a  = roughness * roughness;
	float a2 = a * a;
	float d  = max(dot(n, h), 0.0);
	float d2 = d * d;
	float denom = d2 * (a2 - 1.0) + 1.0;
	return a2 / (PI * denom * denom);
}

float GeometrySchlickGGX(float ndotv, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0;
	return ndotv / (ndotv * (1.0 - k) + k);
}

float GeometrySmith(vec3 n, vec3 v, vec3 l, float roughness)
{
	return GeometrySchlickGGX(max(dot(n, v), 0.0), roughness)
	     * GeometrySchlickGGX(max(dot(n, l), 0.0), roughness);
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 CalcPBRLight(vec3 albedo, float metallic, float roughness, vec3 F0,
                  vec3 n, vec3 v, vec3 l, vec3 radiance)
{
	vec3  h     = normalize(v + l);
	float NDF   = DistributionGGX(n, h, roughness);
	float G     = GeometrySmith(n, v, l, roughness);
	vec3  F     = FresnelSchlick(max(dot(h, v), 0.0), F0);
	vec3  kD    = (1.0 - F) * (1.0 - metallic);
	float NdotL = max(dot(n, l), 0.0);
	vec3  spec  = NDF * G * F / (4.0 * max(dot(n, v), 0.0) * NdotL + 0.0001);
	return (kD * albedo / PI + spec) * radiance * NdotL;
}

void main()
{
	vec2 uv = texCoordFrag * material.tiling;

	vec4  albedoSample = has_albedoMap ? texture(albedoMap, uv) : vec4(1.0);
	vec3  albedo       = material.diffuse * albedoSample.rgb;

	float metallic  = material.metallic;
	float roughness = material.roughness;
	if (has_metallicRoughnessMap) {
		vec2 mr  = texture(metallicRoughnessMap, uv).bg; // glTF: B=metallic, G=roughness
		metallic  *= mr.x;
		roughness *= mr.y;
	}
	roughness = clamp(roughness, 0.04, 1.0);

	// Normal
	vec3 Tn = normalize(T);
	vec3 Nn = normalize(N);
	vec3 Bn = normalize(B);
	mat3 TBN = mat3(Tn, Bn, Nn);
	vec3 norm;
	if (has_normalMap) {
		vec3 tn = texture(normalMap, uv).rgb;
		tn.g    = 1.0 - tn.g;
		tn      = normalize(tn * 2.0 - 1.0);
		norm    = normalize(TBN * tn);
	} else {
		norm = normalize(mat3(transpose(inverse(modelMatrix))) * Nn);
	}

	vec3  F0 = mix(vec3(0.04), albedo, metallic);
	vec3  v  = normalize(viewPos - vertexPositionFrag);
	vec3  result = vec3(0.0);

	for (int i = 0; i < sunLightNum; i++) {
		vec3 l        = normalize(-sunLights[i].direction);
		vec3 radiance = sunLights[i].diffuse * sunLights[i].power;
		result += CalcPBRLight(albedo, metallic, roughness, F0, norm, v, l, radiance);
	}

	for (int i = 0; i < pointLightNum; i++) {
		vec3  l    = normalize(pointLights[i].position - vertexPositionFrag);
		float dist = length(pointLights[i].position - vertexPositionFrag);
		float att  = pointLights[i].power /
		             (pointLights[i].constant + pointLights[i].linear * dist
		              + pointLights[i].quadratic * dist * dist);
		result += CalcPBRLight(albedo, metallic, roughness, F0, norm, v, l,
		                       pointLights[i].diffuse * att);
	}

	for (int i = 0; i < spotLightNum; i++) {
		vec3  l       = normalize(spotLights[i].position - vertexPositionFrag);
		float theta   = dot(l, normalize(-spotLights[i].direction));
		float eps     = spotLights[i].cutOff - spotLights[i].outerCutOff;
		float intens  = clamp((theta - spotLights[i].outerCutOff) / eps, 0.0, 1.0);
		float dist    = length(spotLights[i].position - vertexPositionFrag);
		float att     = spotLights[i].power * intens /
		                (spotLights[i].constant + spotLights[i].linear * dist
		                 + spotLights[i].quadratic * dist * dist);
		result += CalcPBRLight(albedo, metallic, roughness, F0, norm, v, l,
		                       spotLights[i].diffuse * att);
	}

	// Ambient
	vec3 ambient = sceneAmbient * material.ambient * albedo;
	if (skyboxAmbientEnabled)
		ambient += texture(skyboxMap, norm).rgb * skyboxAmbientStrength * material.ambient * albedo;

	float ao = has_aoMap ? texture(aoMap, uv).r : 1.0;

	vec4 emissive = has_emissiveMap ? texture(emissiveMap, uv) : vec4(0.0);

	result = ambient * ao + result + emissive.rgb;

	fragColor = vec4(result, albedoSample.a);
}
