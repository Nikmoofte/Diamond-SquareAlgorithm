#version 330

layout (location = 0) in vec3 aPos;

uniform mat4 proj;
uniform mat4 model;
uniform mat4 view;

out float height;
void main()
{
    gl_Position = proj * view * model * vec4(aPos, 1.0f);
    height = aPos.y;
}