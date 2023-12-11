#version 410

uniform vec3 light_position;
uniform vec3 ambient_color;

in VS_OUT {
	vec3 vertex;
	vec3 normal;
} fs_in;

out vec4 frag_color;

void main()
{
	vec3 L = normalize(light_position - fs_in.vertex);
	vec3 diffuse_color = ambient_color;
	vec3 diffuse = diffuse_color * max(dot(normalize(fs_in.normal), L), 0.0);
	//frag_color = vec4(1.0) * clamp(dot(normalize(fs_in.normal), L), 0.0, 1.0);
	frag_color = 0.4 * vec4(ambient_color, 1.0) +  vec4(diffuse , 1.0) + vec4(1.0) * clamp(dot(normalize(fs_in.normal), L), 0.0, 1.0);
	//frag_color = vec4(diffuse , 1.0) + vec4(1.0) * max(dot(normalize(fs_in.normal), L), 0.0, 1.0);
}
