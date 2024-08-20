#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cerrno>

std::string get_file_contents(const char *filename);

class Shader
{
public:
    GLuint ID;
    Shader(const char *vertex_file, const char *fragment_file);
    Shader();
    void Activate();
    void Delete();
};
#endif