#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;

layout(location = 0) out vec2 texCoords;

layout (binding = 0) uniform UBO
{
    mat4 mvp;
} u_UBO;

void main()
{
    gl_Position = u_UBO.mvp * vec4(a_Position, 1.0);
    texCoords = a_TexCoords;
}
