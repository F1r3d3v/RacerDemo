#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec2 aInstancePosition;
layout (location = 3) in vec2 aInstanceScale;
layout (location = 4) in vec4 aInstanceColor;

out vec2 FragCoord;
out vec4 Color;

uniform vec2 Viewport;

void main()
{
    vec2 coord = aInstancePosition.xy / Viewport;
    coord.y = (1.0 - coord.y);
    coord.xy = coord.xy * 2.0 - 1.0;
    vec2 pos = aPos * aInstanceScale + coord;
    gl_Position = vec4(pos, 0.0, 1.0);
    FragCoord = aTexCoord;
    Color = aInstanceColor;
}