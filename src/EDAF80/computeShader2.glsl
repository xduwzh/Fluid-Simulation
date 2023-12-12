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

void main() {
    // 获取线程（Invocation）的全局索引
    uint particleIndex = gl_GlobalInvocationID.x;
    if (particleIndex >= numParticles) return;
    // ... 这里是ExternalForces的GLSL实现。函数1
    particles[particleIndex].velocities += ExternalForces(particles[particleIndex].positions, particles[particleIndex].velocities) * deltaTime;
    // Predict
    //const float predictionFactor = 1.0 / 120.0;
    float predictionFactor = 1.0 / 120.0;
    particles[particleIndex].predictedPosition = particles[particleIndex].positions + particles[particleIndex].velocities * predictionFactor;
  
    //处理碰撞，这也需要转换   函数2  我感觉跟后面的重复了
    //HandleCollisions(particleIndex); // 根据HLSL中的实现转换为GLSL
   
    //UpdateSpatialHash。函数3
    // 重置偏移量
    particles[particleIndex].SpatialOffsets = int(numParticles);
    // 更新索引缓冲区
    uint index = particleIndex;
    ivec2 cell = GetCell2D(particles[index].predictedPosition, smoothingRadius); 
    uint hash = HashCell2D(cell);
    uint key = KeyFromHash(hash, numParticles);
    particles[particleIndex].SpatialIndices = uvec3(index, hash, key); // 使用 uvec3，因为 GLSL 中没有 uint3
    // 其他Kernel函数也需要以类似的方式转换
    
    //CalculateDensities 函数4
    vec2 pos = particles[particleIndex].predictedPosition; // 取 vec4 的前两个分量作为位置
    particles[particleIndex].Densities = CalculateDensity(pos);
    
    //CalculatePressureForce函数5
    float density = particles[particleIndex].Densities.x; // 使用 .x 替代 [0]
    float densityNear = particles[particleIndex].Densities.y; // 使用 .y 替代 [1]
    float pressure = PressureFromDensity(density);
    float nearPressure = NearPressureFromDensity(densityNear);
    vec2 pressureForce = vec2(0.0);

    //vec2 pos = particles[particleIndex].predictedPosition;
    ivec2 originCell = GetCell2D(pos, smoothingRadius);
    float sqrRadius = smoothingRadius * smoothingRadius;

    // 邻域搜索
    for (int i = 0; i < 9; i++) {
        uint hash = HashCell2D(originCell + offsets2D[i]);
        uint key = KeyFromHash(hash, numParticles);
        uint currIndex = particles[key].SpatialOffsets;

        while (currIndex < numParticles) {
            uvec3 indexData = particles[currIndex].SpatialIndices;
            currIndex++;
            // 如果不再查看正确的箱子则退出
            if (indexData.z != key) break; // 使用 .z 替代 [2]
            // 如果哈希不匹配则跳过
            if (indexData.y != hash) continue; // 使用 .y 替代 [1]

            uint neighbourIndex = indexData.x; // 使用 .x 替代 [0]
            // 如果是自己则跳过
            if (neighbourIndex == particleIndex) continue;

            vec2 neighbourPos = particles[neighbourIndex].predictedPosition;
            vec2 offsetToNeighbour = neighbourPos - pos;
            float sqrDstToNeighbour = dot(offsetToNeighbour, offsetToNeighbour);

            // 如果不在半径内则跳过
            if (sqrDstToNeighbour > sqrRadius) continue;

            // 计算压力
            float dst = sqrt(sqrDstToNeighbour);
            vec2 dirToNeighbour = dst > 0.0 ? offsetToNeighbour / dst : vec2(0.0, 1.0);

            float neighbourDensity = particles[neighbourIndex].Densities.x;
            float neighbourNearDensity = particles[neighbourIndex].Densities.y;
            float neighbourPressure = PressureFromDensity(neighbourDensity);
            float neighbourNearPressure = NearPressureFromDensity(neighbourNearDensity);

            float sharedPressure = (pressure + neighbourPressure) * 0.5;
            float sharedNearPressure = (nearPressure + neighbourNearPressure) * 0.5;

            pressureForce += dirToNeighbour * DensityDerivative(dst, smoothingRadius) * sharedPressure / neighbourDensity;
            pressureForce += dirToNeighbour * NearDensityDerivative(dst, smoothingRadius) * sharedNearPressure / neighbourNearDensity;
        }
    }

    vec2 acceleration = pressureForce / density;
    particles[particleIndex].velocities += acceleration * deltaTime; // 更新速度，使用 .xy 替代 [0]
    
    // CalculateViscosity 函数6
    //vec2 pos = particles[particleIndex].predictedPosition;
    //ivec2 originCell = GetCell2D(pos, smoothingRadius);
    //float sqrRadius = smoothingRadius * smoothingRadius;

    vec2 viscosityForce = vec2(0.0);
    vec2 velocity = particles[particleIndex].velocities; // 获取 vec4 的前两个分量（假设是 vec4 类型）

       for (int i = 0; i < 9; i++) {
           uint hash = HashCell2D(originCell + offsets2D[i]);
           uint key = KeyFromHash(hash, numParticles);
           uint currIndex = particles[key].SpatialOffsets;

           while (currIndex < numParticles) {
               uvec3 indexData = particles[currIndex].SpatialIndices;
               currIndex++;
               // 如果不再查看正确的箱子则退出
               if (indexData.z != key) break; // 使用 .z 替代 [2]
               // 如果哈希不匹配则跳过
               if (indexData.y != hash) continue; // 使用 .y 替代 [1]

               uint neighbourIndex = indexData.x; // 使用 .x 替代 [0]
               // 如果是自己则跳过
               if (neighbourIndex == particleIndex) continue;

               vec2 neighbourPos = particles[neighbourIndex].predictedPosition;
               vec2 offsetToNeighbour = neighbourPos - pos;
               float sqrDstToNeighbour = dot(offsetToNeighbour, offsetToNeighbour);

               // 如果不在半径内则跳过
               if (sqrDstToNeighbour > sqrRadius) continue;

               float dst = sqrt(sqrDstToNeighbour);
               vec2 neighbourVelocity = particles[neighbourIndex].velocities;
               viscosityForce += (neighbourVelocity - velocity) * ViscosityKernel(dst, smoothingRadius);
           }
       }
       particles[particleIndex].velocities += viscosityForce * viscosityStrength * deltaTime;
    
    //UpdatePositions 函数7
    // 更新位置
    particles[particleIndex].positions += particles[particleIndex].velocities * deltaTime;
    // 处理碰撞
    HandleCollisions(particleIndex);
}
