#ifndef MESH_H
#define MESH_H

#include <string>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include "user/VAO.h"
#include "user/EBO.h"
#include "user/texture.h"
#include "user/camera.h"

class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

    VAO VAO;
    Shader shader;

    Mesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, std::vector<Texture> &textures);
    void Draw(glm::mat4 model, Shader &shader);
    void Draw(Shader &shader, Camera &camera, glm::mat4 model);
    void update_shadow_uniforms(glm::mat4 light_projection_matrix, GLuint shadow_map, glm::vec3 lightPos, glm::vec4 lightColor, Shader &shader);
};

#endif