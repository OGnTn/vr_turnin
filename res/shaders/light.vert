#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 camMatrix;

layout (std140) uniform CameraMatrices
{
    mat4 cam;
};

void main()
{
    gl_Position = cam * model * vec4(aPos, 1.0);
}