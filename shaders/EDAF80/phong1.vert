#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec3 Tangent;
out vec3 Bitangent;

uniform mat4 vertex_model_to_world;
uniform mat4 normal_model_to_world;
uniform mat4 vertex_world_to_clip;

void main()
{
    //gl_Position = projection * view * model * vec4(aPos, 1.0);
    //FragPos = vec3(model * vec4(aPos, 1.0));
    //Normal = mat3(transpose(inverse(model))) * aNormal;
    //TexCoords = aTexCoords;
    //Tangent = mat3(model) * aTangent;
    //Bitangent = mat3(model) * aBitangent;

	gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(vertex, 1.0);
	FragPos = vec3(vertex_model_to_world * vec4(aPos, 1.0));
	Normal = vec3(normal_model_to_world * vec4(aNormal, 0));
	TexCoords = aTexCoords;
	Tangent = vec3(vertex_model_to_world * vec4(aTangent, 0.0));
    Bitangent = vec3(vertex_model_to_world * vec4(aBitangent, 0.0));
}
