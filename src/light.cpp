#include "user/light.h"

Light::Light(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec4 light_color) : shader("../res/shaders/light.vert", "../res/shaders/light.frag"), Node3D(position, rotation, scale, shader, "")
{
    this->meshes = std::vector<Mesh>();
    this->light_color = light_color;
    this->shader.Activate();
    glUniform4f(glGetUniformLocation(shader.ID, "lightColor"), light_color.x, light_color.y, light_color.z, light_color.w);
}
Light::Light(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec4 light_color, std::string mesh_path) : shader("../res/shaders/light.vert", "../res/shaders/light.frag"), Node3D(position, rotation, scale, shader, mesh_path)
{
    // this->meshes = std::vector<Mesh>();
    this->light_color = light_color;
    this->shader.Activate();
    glUniform4f(glGetUniformLocation(shader.ID, "lightColor"), light_color.x, light_color.y, light_color.z, light_color.w);
}
