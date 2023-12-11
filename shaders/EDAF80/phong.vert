#version 410

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 binormal;

uniform mat4 vertex_model_to_world;
uniform mat4 normal_model_to_world;
uniform mat4 vertex_world_to_clip;

uniform vec3 light_position;
uniform vec3 camera_position;

out vec3 Normal_vector;
out vec3 View_vector;
out vec3 Light_vector;
out vec2 texCoodinates;
out vec3 Tangent;
out vec3 Binormal;

void main()
{
	vec3 worldPos = vec3(vertex_model_to_world * vec4(vertex, 1.0));
	Normal_vector = vec3(normal_model_to_world * vec4(normal, 0.0));
	View_vector = camera_position - worldPos;
	Light_vector = light_position - worldPos;
	texCoodinates = texCoords;
	Tangent = vec3(vertex_model_to_world * vec4(tangent, 0.0));
	Binormal = vec3(vertex_model_to_world * vec4(binormal, 0.0));
	gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(vertex, 1.0); 
}
