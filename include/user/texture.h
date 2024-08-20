#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb/stb_image.h"
#include "shader.h"

class Texture
{
public:
    GLuint ID;
    const char *type;
    GLuint unit;

    Texture(const char *image, const char *tex_type, GLuint slot, GLenum format, GLenum pixel_type);
    Texture(unsigned char *data, int width, int height, int channel_count, const char *tex_type, GLuint slot, GLenum format, GLenum pixel_type);
    Texture();

    void texUnit(Shader &shader, const char *uniform, GLuint unit);
    void Bind();
    void Unbind();
    void Delete();
};

#endif