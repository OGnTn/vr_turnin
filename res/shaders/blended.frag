#version 330 core

out vec4 FragColor;
in vec2 texCoord;

uniform sampler2D diffuse0;

void main()
{
    if(texture(diffuse0, texCoord).a < 0.05)
        discard;

    FragColor = texture(diffuse0, texCoord);
}