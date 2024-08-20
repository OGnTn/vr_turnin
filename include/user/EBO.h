#ifndef EB0_H
#define EB0_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>



class EBO
{
    public:
        GLuint ID;
        EBO(std::vector<GLuint>& indices);
        void Bind();
        void Unbind();
        void Delete();
};

#endif