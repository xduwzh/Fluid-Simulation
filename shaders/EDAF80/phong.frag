#version 410

in vec3 Normal_vector;
in vec3 View_vector;
in vec3 Light_vector;
in vec2 texCoodinates;
in vec3 Tangent;
in vec3 Binormal;

out vec4 fColor;

uniform sampler2D Phong_texture;
uniform sampler2D Phong_normalmap;
uniform sampler2D Phong_rough;

uniform bool use_normal_mapping;
uniform vec3 ambient_colour;
uniform vec3 diffuse_colour;
uniform vec3 specular_colour;
uniform float shininess_value;

void main()
{
	//vec3 N = normalize(Normal_vector);
	//vec3 V = normalize(View_vector);
	//vec3 L = normalize(Light_vector);
	//vec3 R = normalize(reflect(-L, N));
	//vec3 diffuse = diffuse_colour * max(dot(N, L), 0.0);
	//vec3 specular = specular_colour * pow(max(dot(R,V), 0.0), shininess_value);
	//fColor.xyz  = ambient_colour + diffuse + specular;
	//fColor.w = 1.0;
	
    //vec3 N = texture(Phong_normalmap, texCoodinates).rgb;
	//N = normalize(N * 2.0 - 1.0);
	//N = normalize(Tangent * N.x + Binormal * N.y + Normal_vector * N.z);
	vec3 N;
    if(use_normal_mapping == true){
		N = texture(Phong_normalmap, texCoodinates).rgb;
		N = normalize(N * 2.0 - 1.0);
		N = normalize(Tangent * N.x + Binormal * N.y + Normal_vector * N.z);
	}
	else{
		N = normalize(Normal_vector);
	}

	vec3 V = normalize(View_vector);
	vec3 L = normalize(Light_vector);
	vec3 R = normalize(reflect(-L, N));
	vec3 ambient = 0.2 * texture(Phong_texture, texCoodinates).rgb;
	vec3 diffuse = max(dot(N, L), 0.0) * texture(Phong_texture, texCoodinates).rgb;
	vec3 specular = pow(max(dot(R,V), 0.0), shininess_value) * texture(Phong_rough, texCoodinates).rgb;

	fColor.xyz  = ambient + diffuse + specular;

	fColor.w = 1.0;
}
