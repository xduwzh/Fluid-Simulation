#version 410

in vec2 texCoodinates;
in vec3 View_vector;
in vec3 Light_vector;
in vec3 Normal_vector;

in vec2 normalCoord0;
in vec2 normalCoord1;
in vec2 normalCoord2;

in vec3 Tangent;
in vec3 Bitangent;

uniform vec3 ambient_color;
uniform vec4 deep_color;
uniform vec4 shallow_color;

uniform samplerCube cubemap;
uniform sampler2D wave_texture;

out vec4 frag_color;

void main()
{
	//return amplitude* pow(sin((p.x * direction.x + p.y * direction.y) * frequency + phare_constant * time) * 0.5f + 0.5f, sharpness);
	//n_bump = vec3(normal_model_to_world * vec4(normalize(Tangent * n_bump.x + Bitangent * n_bump.y + Normal_vector * n_bump.z).rgb, 0));

	vec3 V = normalize(View_vector);
	vec3 N = normalize(Normal_vector);

	vec3 n_bump = normalize((texture(wave_texture, normalCoord0).rgb * 2 - 1) + (texture(wave_texture, normalCoord1).rgb * 2 - 1) + (texture(wave_texture, normalCoord2).rgb * 2 - 1));
	mat3 TBN = mat3(Tangent, Bitangent, Normal_vector);
	n_bump = normalize(TBN * n_bump);
	//n_bump = normalize(Tangent * n_bump.x + Bitangent * n_bump.y + Normal_vector * n_bump.z);

	N = n_bump;

	float R0 = 0.02037f;
	float fresnel = R0 + (1 - R0) * pow(1 - dot(V, N), 5);

	vec3 R = normalize(reflect(-V, N));
	vec3 reflection = texture(cubemap, R).rgb;

	float eta = 1.0f / 1.33f;
	//vec3 refraction = refract(-Light_vector, N, eta);
	vec3 refraction = normalize(refract(R, N, eta));
	refraction = texture(cubemap, refraction).rgb;

	float facing = 1 - max(dot(V, N), 0);
	vec4 water_color = mix(deep_color, shallow_color, facing);

	frag_color = water_color + vec4(reflection, 1.0) * fresnel + vec4(refraction, 1.0) * (1 - fresnel);
	//frag_color = vec4(refraction, 1.0) * (1 - fresnel);
}
