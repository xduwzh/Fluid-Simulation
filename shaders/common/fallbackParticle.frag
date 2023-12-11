#version 410

in vec2 paticleVelocity;
out vec4 frag_color;

void main()
{
    vec4 red = vec4(1.0, 0.0, 0.0, 1.0);
	vec4 blue = vec4(0.0, 0.0, 1.0, 1.0);
	float speedRate = length(paticleVelocity)/6.5;
	vec3 interpColor = vec3(speedRate, 1 - speedRate, 0.0);
	frag_color = vec4(interpColor,1.0);
}
