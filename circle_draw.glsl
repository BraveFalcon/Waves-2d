#version 440 core

layout (local_size_x = 10, local_size_y = 10) in;

layout( binding = 0, rgba32f) uniform image2D DataTexture;

uniform int x_center, y_center;
uniform int mode; //0: закрепление, -1: опускание, 1: подъем

void main() {
    int x = int(gl_GlobalInvocationID.x) + 1;
    int y = int(gl_GlobalInvocationID.y) + 1;
    if (mode == 0){
        if (x == x_center && y == y_center){
            vec4 prev_state = imageLoad(DataTexture, ivec2(x, y));
            imageStore(DataTexture, ivec2(x, y), vec4(prev_state.xy, 0.0f, 2.0f));
        }
    }
    else{
        float dist = length(vec2(x, y) - vec2(x_center, y_center));
        if (dist < 5){
            float h = mode * 2 / (1 + 0.5*dist);
            imageStore(DataTexture, ivec2(x, y), vec4(h, h, 0.0f, 0.0f));
        }
    }
}
