#version 330 core

#define PI 3.1415926
#define EPSILON 0.000001

out vec4 FragColor;

uniform vec3 point_light_pos;
uniform vec3 point_light_radiance;
uniform sampler2D shadow_map;				
uniform bool have_shadow;

uniform mat4 light_space_matrix;

uniform float ambient;
uniform float specular;
uniform sampler2D color_texture;

uniform vec3 camera_pos;

in VS_OUT {
    vec3 WorldPos;
    vec2 TexCoord;
    vec3 Normal;
} vs_out;

float ShadowCalculation(vec4 fragPosLightSpace)
{
	// TODO 2.2.2 : Complete the following shadow mapping calculations
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	
	projCoords = projCoords * 0.5 + 0.5;
	
	if(projCoords.z > 1.0)
		return 0.0;
		
	float closestDepth = texture(shadow_map, projCoords.xy).r;
	
	float currentDepth = projCoords.z;
	float bias = 0.005;
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

	return shadow;
}

void main() {
	vec3 color = texture(color_texture, vs_out.TexCoord).rgb;

	vec4 fragPosLightSpace = light_space_matrix * vec4(vs_out.WorldPos, 1.0);

	// 计算阴影
	float shadow = 0.0;
	if(have_shadow) {
		shadow = ShadowCalculation(fragPosLightSpace);
	}

	// TODO 2.2.1 : Complete the Phong shading calculations and modify the output
	
	// 归一化法线
	vec3 normal = normalize(vs_out.Normal);
	
	// 计算光线方向（从片段指向光源）
	vec3 light_dir = normalize(point_light_pos - vs_out.WorldPos);
	
	// 计算视线方向（从片段指向相机）
	vec3 view_dir = normalize(camera_pos - vs_out.WorldPos);
	
	// 计算半程向量（用于Blinn-Phong）
	vec3 half_dir = normalize(light_dir + view_dir);
	
	// 环境光分量
	vec3 ambient_color = ambient * color;
	
	// 漫反射分量
	float diff = max(dot(normal, light_dir), 0.0);
	vec3 diffuse_color = diff * color;
	
	// 镜面反射分量 (Blinn-Phong模型)
	float spec = pow(max(dot(normal, half_dir), 0.0), 32.0);
	vec3 specular_color = specular * spec * vec3(1.0);
	
	// 计算光线衰减
	float distance = length(point_light_pos - vs_out.WorldPos);
	float attenuation = 1.0 / (distance * distance);
	
	// 组合所有光照分量，应用阴影和衰减
	vec3 lighting = ambient_color + 
				   (1.0 - shadow) * (diffuse_color + specular_color) * 
				   point_light_radiance * attenuation;
	
	FragColor = vec4(lighting, 1.0);
}