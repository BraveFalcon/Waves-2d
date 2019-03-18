#version 440 core

layout (local_size_x = 10, local_size_y = 10) in;

layout( binding = 0, rgba32f) uniform image2D DataTexture;

uniform int step;

uniform float dt=0.001;

void main() {
    int x = int(gl_GlobalInvocationID.x) + 1;
    int y = int(gl_GlobalInvocationID.y) + 1;

    vec4 data = imageLoad(DataTexture, ivec2(x, y));
    if (data[3] != 2.0f){
        float acceleration = imageLoad(DataTexture, ivec2(x, y + 1))[step] +
                             imageLoad(DataTexture, ivec2(x + 1, y))[step] +
                             imageLoad(DataTexture, ivec2(x, y - 1))[step] +
                             imageLoad(DataTexture, ivec2(x - 1, y))[step] -
                             4.0f * data[step] - 0 * data[2];

        data[2] += acceleration * dt;
        data[(step + 1) % 2] += data[2] * dt;

        imageStore(DataTexture, ivec2(x, y), data);
    }
}
