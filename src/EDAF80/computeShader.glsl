#version 430 core

layout(binding = 0, std430) buffer PositionaBuffer {
    vec2 positions[];
};
layout(binding = 1, std430) buffer VelocitiesBuffer {
    vec2 velocities[];
};
layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;
void main() {
    uint index = gl_GlobalInvocationID.x;
    //positions[index] += vec2(0.0, 0.01);
    positions[index] += velocities[index];
}
