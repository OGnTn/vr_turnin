#include "user/mesh.h"

Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, std::vector<Texture> &textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    // std::cout << textures.size() << std::endl;
    this->shader = Shader("../res/shaders/def.vert", "../res/shaders/def.vert");

    VAO.Bind();

    VBO VBO(vertices);
    EBO EBO(indices);

    VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void *)0);
    VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, normal));
    VAO.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, color));
    VAO.LinkAttrib(VBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, tex_coords));

    VAO.Unbind();
    VBO.Unbind();
    EBO.Unbind();
}

void Mesh::update_shadow_uniforms(glm::mat4 light_projection_matrix, GLuint shadow_map, glm::vec3 lightPos, glm::vec4 lightColor, Shader &shader)
{
    shader.Activate();
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "lightProjection"), 1, GL_FALSE, glm::value_ptr(light_projection_matrix));
    glUniform3f(glGetUniformLocation(shader.ID, "dir_light_pos"), lightPos.x, lightPos.y, lightPos.z);
    if (glGetError() != 0)
    {
        std::cout << "light pos: " << glGetError() << std::endl;
    }
    glUniform4f(glGetUniformLocation(shader.ID, "dir_light_color"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    if (glGetError() != 0)
    {
        std::cout << "light color: " << glGetError() << std::endl;
    }
    glUniform1i(glGetUniformLocation(shader.ID, "shadowMap"), 2);
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, shadow_map);
    GLenum error = glGetError();
    if (error != 0)
    {
        std::cout << "shadow tex: " << error << std::endl;
    }
}

void Mesh::Draw(glm::mat4 model, Shader &shader)
{
    // this->shader.Activate();
    VAO.Bind();

    int numDiffuse = 0;
    int numSpecular = 0;

    for (unsigned int i = 0; i < textures.size(); i++)
    {
        std::string number;
        std::string type = textures[i].type;

        if (type == "diffuse")
            number = std::to_string(numDiffuse++);
        else if (type == "specular")
            number = std::to_string(numSpecular++);

        textures[i].texUnit(shader, (type + number).c_str(), i);
        // std::cout << "Texture type: " << type << " number: " << number << std::endl;
        textures[i].Bind();
        GLenum error = glGetError();
        if (error != 0)
        {
            std::cout << "mesh bind tex  " << i << error << std::endl;
        }
    }

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    GLenum error = glGetError();
    if (error != 0)
    {
        std::cout << "mesh draw: " << error << std::endl;
    }
}

void Mesh::Draw(Shader &shader, Camera &camera, glm::mat4 model)
{
    // print the model matrix
    // std::cout << glm::to_string(model) << std::endl;
    shader.Activate();
    GLuint modelLoc = glGetUniformLocation(shader.ID, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    GLenum error = glGetError();
    if (error != 0)
    {
        std::cout << "mesh model uniform: " << error << std::endl;
    }
    VAO.Bind();

    int numDiffuse = 0;
    int numSpecular = 0;

    for (unsigned int i = 0; i < textures.size(); i++)
    {
        std::string number;
        std::string type = textures[i].type;

        if (type == "diffuse")
            number = std::to_string(numDiffuse++);
        else if (type == "specular")
            number = std::to_string(numSpecular++);

        textures[i].texUnit(shader, (type + number).c_str(), i);
        // std::cout << "Texture type: " << type << " number: " << number << std::endl;
        textures[i].Bind();
        // std::cout << "Mesh bind texture " << i << ": " << glGetError() << std::endl;
    }

    // glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
    // camera.Matrix(shader, "camMatrix");
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    error = glGetError();
    if (error != 0)
    {
        std::cout << "mesh draw2: " << error << std::endl;
    }
}