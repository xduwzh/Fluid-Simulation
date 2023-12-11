#version 430 core

struct particleParameters{
    vec2 positions;
    vec2 velocities;
};

layout(binding = 0, std430) buffer dataBuffer {
    particleParameters particles[];
};
uniform float collisionDamping;
uniform float gravity;
uniform float particleRadius;
uniform float deltaTime;
uniform vec2 boundsSize;

layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;
void main() {
    uint index = gl_GlobalInvocationID.x;
    //positions[index] += vec2(0.0, 0.01);
    //particles[index].positions += particles[index].velocities;
    vec2 pos = particles[index].positions;
    vec2 vel = particles[index].velocities;

    // 保持粒子在边界内
    const vec2 halfSize = boundsSize * 0.5;
    vec2 edgeDst = halfSize - abs(pos);

    if (edgeDst.x <= 0.0)
    {
        pos.x = halfSize.x * sign(pos.x);
        vel.x *= -1.0 * collisionDamping;
    }
    if (edgeDst.y <= 0.0)
    {
        pos.y = halfSize.y * sign(pos.y);
        vel.y *= -1.0 * collisionDamping;
    }
    // 更新位置和速度
    
    particles[index].positions = pos;
    particles[index].velocities = vel;
    particles[index].positions += particles[index].velocities * deltaTime;
    particles[index].velocities += vec2(0, gravity) * deltaTime;
}
