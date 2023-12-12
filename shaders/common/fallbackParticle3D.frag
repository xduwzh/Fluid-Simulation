#version 410

in vec3 paticleVelocity;
out vec4 frag_color;

void main()
{
    vec4 red = vec4(1.0, 0.0, 0.0, 1.0);
	vec4 blue = vec4(0.0, 0.0, 1.0, 1.0);
	float speedRate = length(paticleVelocity)/6.5;
	vec3 interpColor = vec3(speedRate, 0.5 - 0.5 * speedRate, 1.0 - speedRate);
	frag_color = vec4(interpColor,1.0);
}
