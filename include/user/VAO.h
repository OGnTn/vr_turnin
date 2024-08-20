#ifndef VAO_H
#define VAO_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "VBO.h"

class VAO
{
public:
    GLuint ID;
    VAO();
    void LinkAttrib(VBO &VBO, GLuint layout, GLuint component_count, GLenum type, GLsizeiptr stride, void *offset);
    void Bind();
    void Unbind();
    void Delete();
};

#endif