#include "user/shader.h"

std::string get_file_contents(const char *filename)
{
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (in)
    {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return (contents);
    }
    throw(errno);
}
Shader::Shader()
{
    std::string vertex_code = get_file_contents("../res/shaders/def.vert");
    std::string fragment_code = get_file_contents("../res/shaders/def.vert");

    const char *vertex_shader_code = vertex_code.c_str();
    const char *fragment_shader_code = fragment_code.c_str();

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_code, NULL);
    glCompileShader(vertex_shader);

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_code, NULL);
    glCompileShader(fragment_shader);

    ID = glCreateProgram();
    glAttachShader(ID, vertex_shader);
    glAttachShader(ID, fragment_shader);
    glLinkProgram(ID);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

Shader::Shader(const char *vertex_file, const char *fragment_file)
{
    std::string vertex_code = get_file_contents(vertex_file);
    std::string fragment_code = get_file_contents(fragment_file);

    const char *vertex_shader_code = vertex_code.c_str();
    const char *fragment_shader_code = fragment_code.c_str();

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_code, NULL);
    glCompileShader(vertex_shader);

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_code, NULL);
    glCompileShader(fragment_shader);

    ID = glCreateProgram();
    glAttachShader(ID, vertex_shader);
    glAttachShader(ID, fragment_shader);
    glLinkProgram(ID);

    unsigned int uniform_block_index = glGetUniformBlockIndex(ID, "CameraMatrices");
    glUniformBlockBinding(ID, uniform_block_index, 0);
    unsigned int uniform_block_index2 = glGetUniformBlockIndex(ID, "Lights");
    glUniformBlockBinding(ID, uniform_block_index2, 1);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void Shader::Activate()
{
    glUseProgram(ID);
    glUniform1i(glGetUniformLocation(ID, "skybox"), 3);
}

void Shader::Delete()
{
    glDeleteProgram(ID);
}