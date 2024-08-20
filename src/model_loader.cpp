#include "user/model_loader.h"

void ModelLoader::load_model(std::string file_path, std::vector<Mesh> &meshes, bool invert_normals)
{
    std::cout << "Loading model: " << file_path << std::endl;
    Assimp::Importer importer;
    std::vector<std::string> materials_loaded;
    const aiScene *scene = importer.ReadFile(file_path, aiProcess_Triangulate);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    aiMatrix4x4 identity_matrix; // Identity matrix for root node
    process_node(scene->mRootNode, scene, meshes, materials_loaded, identity_matrix, invert_normals);
}

void ModelLoader::process_node(aiNode *node, const aiScene *scene, std::vector<Mesh> &meshes, std::vector<std::string> &materials_loaded, const aiMatrix4x4 &parent_transformation, bool invert_normals)
{
    aiMatrix4x4 node_transformation = parent_transformation * node->mTransformation;

    // std::cout << "Processing node: " << node->mName.C_Str() << std::endl;
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(process_mesh(mesh, scene, node_transformation, materials_loaded, invert_normals));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        process_node(node->mChildren[i], scene, meshes, materials_loaded, node_transformation, invert_normals);
    }
}
glm::mat4 aiMatrixToGlm(const aiMatrix4x4 &aiMat)
{
    return glm::mat4(
        aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1,
        aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2,
        aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3,
        aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4);
}

void apply_transform(glm::vec3 &vertex, const aiMatrix4x4 &transformation)
{
    // Convert aiMatrix4x4 to glm::mat4
    glm::mat4 glm_transform = aiMatrixToGlm(transformation);

    // Convert glm::vec3 to glm::vec4 (homogeneous coordinates)
    glm::vec4 temp(vertex, 1.0f);

    // Apply transformation
    glm::vec4 transformed = glm_transform * temp;

    // Convert back to glm::vec3
    vertex = glm::vec3(transformed);
}

Mesh ModelLoader::process_mesh(aiMesh *mesh, const aiScene *scene, const aiMatrix4x4 &node_transformation, std::vector<std::string> &materials_loaded, bool invert_normals)
{
    // std::cout << "Processing mesh: " << mesh->mName.C_Str() << std::endl;
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

    aiMatrix4x4 mesh_transform = node_transformation;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector;

        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        apply_transform(vector, mesh_transform);
        vertex.position = vector;

        if (mesh->mNormals)
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            apply_transform(vector, mesh_transform);
            if (invert_normals)
            {
                vector = -vector;
            }
            vertex.normal = vector;
        }

        for (unsigned int j = 0; j < AI_MAX_NUMBER_OF_TEXTURECOORDS; j++)
        {
            if (mesh->mTextureCoords[j])
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[j][i].x;
                vec.y = mesh->mTextureCoords[j][i].y;
                vertex.tex_coords = vec;
                break; // Use the first available set
            }
            vertex.tex_coords = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

        unsigned int diffuse_count = material->GetTextureCount(aiTextureType_DIFFUSE);
        unsigned int specular_count = material->GetTextureCount(aiTextureType_SPECULAR);

        for (int diffuse_index = 0; diffuse_index < diffuse_count; diffuse_index++)
        {
            aiString texture_file;
            material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), texture_file);
            if ('*' != texture_file.data[0])
            {
                // external texture
                process_external_texture(material, diffuse_index, aiTextureType_DIFFUSE, textures);
            }
            else
            {
                // internal texture
                process_internal_texture(material, diffuse_index, aiTextureType_DIFFUSE, scene, textures);
            }
        }

        for (int specular_index = 0; specular_index < specular_count; specular_index++)
        {
            aiString texture_file;
            material->Get(AI_MATKEY_TEXTURE(aiTextureType_SPECULAR, 0), texture_file);
            if ('*' != texture_file.data[0])
            {
                // external texture
                process_external_texture(material, specular_index, aiTextureType_DIFFUSE, textures);
            }
            else
            {
                // internal texture
                process_internal_texture(material, specular_index, aiTextureType_DIFFUSE, scene, textures);
            }
        }
    }

    return Mesh(vertices, indices, textures);
}

void ModelLoader::process_external_texture(aiMaterial *mat, int slot, aiTextureType type, std::vector<Texture> &textures)
{
    aiString texture_file;
    mat->Get(AI_MATKEY_TEXTURE(type, 0), texture_file);
    // std::cout << "external texture" << std::endl;
    // std::cout << texture_file.C_Str() << std::endl;
    //    if the path contains "/", assume it is an absolute path

    std::string tex_type;
    if (type == aiTextureType_DIFFUSE)
    {
        tex_type = "diffuse";
    }
    else if (type == aiTextureType_SPECULAR)
    {
        tex_type = "specular";
    }
    else
    {
        tex_type = "diffuse";
    }
    if (strchr(texture_file.C_Str(), '/') != NULL || strchr(texture_file.C_Str(), '\\') != NULL)
    {

        // std::cout << "absolute texture" << std::endl;

        std::string texture_name = texture_file.C_Str();
        size_t pos = texture_name.find_last_of('/');
        if (pos != std::string::npos)
        {
            texture_name = texture_name.substr(pos + 1);
        }
        pos = texture_name.find_last_of('\\');
        if (pos != std::string::npos)
        {
            texture_name = texture_name.substr(pos + 1);
        }

        std::string texture_path = "../res/textures/";
        texture_path += texture_name;
        // std::cout << texture_path << std::endl;

        // check if the file exists
        std::ifstream f(texture_path.c_str());
        if (!f.good())
        {
            // std::cout << "Texture file not found: " << texture_path << std::endl;
            //   modify jpg file extension to jpeg
            std::string::size_type pos = texture_path.rfind('.');
            if (pos != std::string::npos)
            {
                texture_path.replace(pos, 4, ".jpeg");
                // std::cout << "Trying: " << texture_path << std::endl;
                std::ifstream f(texture_path.c_str());
                if (!f.good())
                {
                    // std::cout << "Texture file not found: " << texture_path << std::endl;
                }
                else
                {
                    // std::cout << "Texture file found: " << texture_path << std::endl;
                    Texture tex(texture_path.c_str(), tex_type.c_str(), slot, GL_RGBA, GL_UNSIGNED_BYTE);
                    textures.push_back(tex);
                }
            }
            return;
        }

        Texture tex(texture_path.c_str(), tex_type.c_str(), slot, GL_RGBA, GL_UNSIGNED_BYTE);
        textures.push_back(tex);
    }
    else
    {
        // std::cout << "relative texture" << std::endl;
        // std::cout << texture_file.C_Str() << std::endl;
        //  relative filename. take the last part and look for it in ../res/textures/
        //  split the string and get the part after the last '/'

        std::string texture_path = "../res/textures/";
        texture_path += texture_file.C_Str();
        // std::cout << texture_path << std::endl;
        //  std::cout << "internal texture" << std::endl;
        Texture tex(texture_path.c_str(), tex_type.c_str(), slot, GL_RGBA, GL_UNSIGNED_BYTE);
        textures.push_back(tex);
    }
}

void ModelLoader::process_internal_texture(aiMaterial *mat, int slot, aiTextureType type, const aiScene *scene, std::vector<Texture> &textures)
{
    // << "internal texture" << std::endl;
    aiString texture_file;
    mat->Get(AI_MATKEY_TEXTURE(type, 0), texture_file);

    std::string tex_type;
    if (type == aiTextureType_DIFFUSE)
    {

        // std::cout << "diffuse" << std::endl;
        tex_type = "diffuse";
    }
    else if (type == aiTextureType_SPECULAR)
    {
        tex_type = "specular";
    }
    else
    {
        tex_type = "diffuse";
    }

    unsigned int path = atoi(texture_file.C_Str() + 1);
    aiTexture *t = scene->mTextures[path];
    if (t->mHeight == 0)
    {
        // std::cout << "compressed texture" << std::endl;
        //  compressed texture
        aiTexel *texel = t->pcData;
        unsigned char *rawData = reinterpret_cast<unsigned char *>(texel);

        int width, height, nrChannels;
        stbi_info_from_memory(rawData, t->mWidth, &width, &height, &nrChannels);

        unsigned char *img = stbi_load_from_memory(rawData, t->mWidth, &width, &height, &nrChannels, 0);
        Texture tex(img, width, height, nrChannels, tex_type.c_str(), slot, GL_RGBA, GL_UNSIGNED_BYTE);
        stbi_image_free(img);
        textures.push_back(tex);
    }
    else
    {
        // uncompressed texture
        Texture tex((unsigned char *)t->pcData, t->mWidth, t->mHeight, 4, tex_type.c_str(), slot, GL_RGBA, GL_UNSIGNED_BYTE);
        textures.push_back(tex);
    }
}
