#version 410

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec2 Position2D;
layout (location = 2) in vec2 Velocity;

uniform mat4 vertex_model_to_world;
uniform mat4 vertex_world_to_clip;

out vec2 paticleVelocity;

void main()
{	
	paticleVelocity = Velocity;
	gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(vertex.xy + Position2D, 0.0, 1.0);
	//gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(vertex, 1.0);
}
