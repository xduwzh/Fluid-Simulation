#version 410

uniform samplerCube cubemap;

in vec3 var_tex;

out vec4 frag_color;

void main()
{
	frag_color = texture(cubemap, var_tex);
}
