#version 450

layout(location = 0) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

layout (binding = 1) uniform sampler2D u_Texture;

void main()
{
    outColor = texture(u_Texture, texCoords);
}
