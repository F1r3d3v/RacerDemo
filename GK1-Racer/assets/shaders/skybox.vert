#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

layout (std140) uniform Matrices
{
    mat4 view;
    mat4 projection;
};

void main()
{
    TexCoords = aPos;
    mat4 _view = mat4(mat3(view));
    vec4 pos = projection * _view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}
