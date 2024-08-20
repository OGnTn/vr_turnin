#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "stb/stb_image_write.h"

#include "user/mesh.h"

class ModelLoader
{
public:
    static void load_model(std::string file_path, std::vector<Mesh> &meshes, bool invert_normals = false);
    static void process_node(aiNode *node, const aiScene *scene, std::vector<Mesh> &meshes, std::vector<std::string> &materials_loaded, const aiMatrix4x4 &parent_transformation, bool invert_normals);

    static Mesh process_mesh(aiMesh *mesh, const aiScene *scene, const aiMatrix4x4 &node_transformation, std::vector<std::string> &materials_loaded, bool invert_normals);
    static void process_external_texture(aiMaterial *mat, int slot, aiTextureType type, std::vector<Texture> &textures);
    static void process_internal_texture(aiMaterial *mat, int slot, aiTextureType type, const aiScene *scene, std::vector<Texture> &textures);
};
#endif