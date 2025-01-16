#version 330 core
in vec4 Color;
in vec2 FragCoord;
out vec4 FragColor;

void main()
{
    vec2 uv = FragCoord * 2.0 - 1.0;  
    float d = length(uv);
    float fade = fwidth(d);
    float alpha = 1.0 - smoothstep(1.0 - fade, 1.0 + fade, d);
    FragColor = vec4(Color.rgb, alpha);
}
