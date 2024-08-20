#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;

out vec3 fragPos;
out vec3 Normal;
out vec3 color;
out vec2 texCoord;
out vec3 camPos;
out vec4 fragPosLight;

uniform mat4 lightProjection;
uniform mat4 camMatrix;
uniform mat4 model;

layout (std140) uniform CameraMatrices
{
    mat4 cam;
    vec3 camPosition;
};

struct str_point_light
{
   vec3 position;
   vec4 color;
   float linear;
   float quadratic;
   float constant;
};
//out str_point_light[1] lights;

//layout(std140) uniform Lights 
//{
//   str_point_light ilights[1];
//};



void main()
{
   fragPos = vec3(model * vec4(aPos, 1.0));
   camPos = camPosition;
   gl_Position = cam * vec4(fragPos, 1.0);
   color = aColor;
   texCoord = aTex;
   //Normal = aNormal;
   //lights = ilights;
   Normal = vec3(model * vec4(aNormal, 1.0));
   //fragPosLight = vec4(0.0f, 0.0f, 0.0f, 0.0f);
   fragPosLight = lightProjection * vec4(fragPos, 1.0f);
}