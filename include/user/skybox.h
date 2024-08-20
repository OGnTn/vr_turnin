#ifndef SKYBOX_H
#define SKYBOX_H
#include "user/shader.h"
#include "user/camera.h"
#include "stb/stb_image.h"
#include <vector>
#include <string>

class Skybox
{
public:
    Skybox(std::vector<std::string> faces);
    void draw(Camera &camera);
    unsigned int texture_id;

private:
    Shader shader;
    unsigned int VAO, VBO;
    static unsigned int load_cubemap(std::vector<std::string> faces);
};

#endif