#version 410

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

uniform mat4 vertex_model_to_world;
uniform mat4 normal_model_to_world;
uniform mat4 vertex_world_to_clip;

uniform vec3 camera_position;
uniform vec3 light_position;

uniform float elapsed_time_s;
uniform float amplitude;
uniform float frequency;
uniform float phare_constant;
uniform float sharpness;
uniform vec2 direction;

uniform float amplitude1;
uniform float frequency1;
uniform float phare_constant1;
uniform float sharpness1;
uniform vec2 direction1;

out vec2 texCoodinates;
out vec3 View_vector;
out vec3 Light_vector;
out vec3 Normal_vector;

out vec2 normalCoord0;
out vec2 normalCoord1;
out vec2 normalCoord2;

out vec3 Tangent;
out vec3 Bitangent;

float wave(vec2 p)
{
    return amplitude* pow(sin((p.x * direction.x + p.y * direction.y) * frequency + phare_constant * elapsed_time_s) * 0.5f + 0.5f, sharpness);
}
float wave1(vec2 p)
{
    return amplitude1* pow(sin((p.x * direction1.x + p.y * direction1.y) * frequency1 + phare_constant1 * elapsed_time_s) * 0.5f + 0.5f, sharpness1);
}

float derivative_x(vec2 p){
	return 0.5 * sharpness * frequency * amplitude * pow(sin((direction.x * p.x + direction.y * p.y) * frequency + phare_constant * elapsed_time_s) * 0.5 + 0.5, sharpness - 1) * cos((direction.x * p.x + direction.y * p.y)* frequency + phare_constant * elapsed_time_s) * direction.x;
}

float derivative_z(vec2 p){
	return 0.5 * sharpness * frequency * amplitude * pow(sin((direction.x * p.x + direction.y * p.y) * frequency + phare_constant * elapsed_time_s) * 0.5 + 0.5, sharpness - 1) * cos((direction.x * p.x + direction.y * p.y)* frequency + phare_constant * elapsed_time_s) * direction.y;
}
float derivative_x1(vec2 p){
	return 0.5 * sharpness1 * frequency1 * amplitude1 * pow(sin((direction1.x * p.x + direction1.y * p.y) * frequency1 + phare_constant1 * elapsed_time_s) * 0.5 + 0.5, sharpness1 - 1) * cos((direction1.x * p.x + direction1.y * p.y)* frequency1 + phare_constant1 * elapsed_time_s) * direction1.x;
}

float derivative_z1(vec2 p){
	return 0.5 * sharpness1 * frequency1 * amplitude1 * pow(sin((direction1.x * p.x + direction1.y * p.y) * frequency1 + phare_constant1 * elapsed_time_s) * 0.5 + 0.5, sharpness1 - 1) * cos((direction1.x * p.x + direction1.y * p.y)* frequency1 + phare_constant1 * elapsed_time_s) * direction1.y;
}

void main()
{
	vec4 pos = vec4(vertex, 1.0);
	pos.y = wave(pos.xz) + wave1(pos.xz);
	Normal_vector = vec3(normal_model_to_world * vec4(-derivative_x(pos.xz), 1, -derivative_z(pos.xz), 0)) + vec3(normal_model_to_world * vec4(-derivative_x1(pos.xz), 1, -derivative_z1(pos.xz), 0));

	//Normal_vector = vec3(-derivative_x(pos.xz), 1, -derivative_z(pos.xz));
	//Tangent = normalize(vec3(vertex_model_to_world * vec4(1,derivative_x(pos.xz),0, 0)));
	//Bitangent = normalize(vec3(vertex_model_to_world * vec4(0, derivative_z(pos.xz), 1, 0)));

	vec3 worldPos = vec3(vertex_model_to_world * pos);

	//vec4 pos = vertex_model_to_world * vec4(vertex. 1.0;
	//pos.y = wave(pos.xz);
	//Normal_vector = vec3(normal_model_to_world * vec4(-derivative_x(pos.xz), 1, -derivative_z(pos.xz), 0));
	//worldPos = vec3(pos);

	Light_vector = light_position - worldPos;
	View_vector = camera_position - worldPos;
	
	texCoodinates = texCoords;
	//calculate the animated normal mapping coordinates
	Tangent = normalize(vec3(1,derivative_x1(pos.xz),0));
	Bitangent = normalize(vec3(0, derivative_z1(pos.xz), 1));

	vec2 texScale = vec2(8, 4);
	float normalTime = mod(elapsed_time_s, 100.0);
	vec2 normalSpeed = vec2(-0.05, 0.0);
	normalCoord0.xy = texCoords.xy * texScale + normalTime * normalSpeed;
	normalCoord1.xy = texCoords.xy * texScale * 2 + normalTime * normalSpeed * 4;
	normalCoord2.xy = texCoords.xy * texScale * 4 + normalTime * normalSpeed * 8;
	

	gl_Position = vertex_world_to_clip * vertex_model_to_world * pos;
}
