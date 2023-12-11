#version 430 core

layout(binding = 0, std430) buffer DataBuffer {
    vec2 data[];
};
layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

void main() {
    uint index = gl_GlobalInvocationID.x;
    data[index] += vec2(0.0, -2.0);
}
