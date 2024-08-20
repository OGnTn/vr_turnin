#include "user/point_light.h"

PointLight::PointLight(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec4 light_color, float constant, float linear, float quadratic) : Light(position, rotation, scale, light_color)
{
    this->light_color = light_color;
    this->constant = constant;
    this->linear = linear;
    this->quadratic = quadratic;
}
PointLight::PointLight(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec4 light_color, float constant, float linear, float quadratic, std::string mesh_path) : Light(position, rotation, scale, light_color, mesh_path)
{
    this->light_color = light_color;
    this->constant = constant;
    this->linear = linear;
    this->quadratic = quadratic;
}