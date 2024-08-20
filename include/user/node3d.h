#ifndef NODE3D_H
#define NODE3D_H

#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "user/node.h"
#include "user/mesh.h"
#include "user/shader.h"
#include "user/camera.h"
#include "user/model_loader.h"

struct vector3
{
    float x;
    float y;
    float z;
};

class Node3D : public Node
{
public:
    Node3D(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, Shader &shader, std::string model_file_path, bool invert_normals = false);
    void assign_meshes(std::vector<Mesh> meshes);
    void draw();
    void draw(Shader &shader, Camera &camera);
    void set_position(glm::vec3 position);
    void set_rotation(glm::vec3 rotation);
    void set_scale(glm::vec3 scale);
    void update_shadow_uniforms(glm::mat4 light_projection_matrix, glm::vec3 lightPos, glm::vec4 lightColor, GLuint shadow_map);
    void set_cubemap(GLuint cubemap);
    glm::mat4 model;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    std::vector<Mesh> meshes;

private:
    Shader &shader;
    void update_model();
};

#endif