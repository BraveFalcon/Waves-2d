#version 440 core

layout (location = 0) in vec2 position;

out vec2 texCoord;

void main()
{
    gl_Position = vec4(position.xy, 0.0f, 1.0f);
    texCoord = 0.5f * (position + 1);
    texCoord.y = 1.0f - texCoord.y; //Переворот изображения
}