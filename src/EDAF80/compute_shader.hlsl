// compute_shader.hlsl

RWStructuredBuffer<float3> PositionsBuffer : register(u0);
RWStructuredBuffer<float3> VelocitiesBuffer : register(u1);
float deltaTime : register(c0);

[numthreads(256, 1, 1)]
void UpdatePositions(uint3 id : SV_DispatchThreadID)
{
    if (id.x >= PositionsBuffer.Length)
        return;

    PositionsBuffer[id.x] += VelocitiesBuffer[id.x] * deltaTime;
    // HandleCollisions or any other computation can be done here
}
