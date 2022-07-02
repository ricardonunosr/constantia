#include "model.h"
#include <algorithm>
#include <iostream>
#include <unordered_map>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

void Model::Draw(Shader& shader)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i].Draw(shader);
    }
}

namespace std
{
template <> struct hash<Vertex>
{
    size_t operator()(Vertex const& vertex) const
    {
        return ((hash<glm::vec3>()(vertex.position))) ^ (hash<glm::vec2>()(vertex.texCoords) << 1);
    }
};
} // namespace std

void Model::loadModel(const std::string& path)
{
    directory = path.substr(0, path.find_last_of('/'));

    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = directory;
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(path, reader_config))
    {
        if (!reader.Error().empty())
        {
            spdlog::error("TinyObjReader: {}", reader.Error());
        }
        exit(1);
    }

    if (!reader.Warning().empty())
    {
        spdlog::warn("TinyObjReader: {}", reader.Warning());
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    /*  We are grouping every mesh that was the same material.
     *  This way we can group every mesh into one draw call instead of multiple for the same mesh.
     *  +1 for an unknown material (index 0 is the unknown material)
     */
    std::vector<MeshMaterialGroup> groups(materials.size() + 1);

    for (size_t i = 0; i < materials.size(); i++)
    {
        if (materials[i].diffuse_texname != "")
        {
            std::string diffusePath(materials[i].diffuse_texname);
            std::replace(diffusePath.begin(), diffusePath.end(), '\\', '/');
            groups[i + 1].diffuse_path = directory + "/" + diffusePath;
        }
        if (materials[i].specular_texname != "")
        {
            std::string specularPath(materials[i].specular_texname);
            std::replace(specularPath.begin(), specularPath.end(), '\\', '/');
            groups[i + 1].specular_path = directory + "/" + specularPath;
        }
        else if (materials[i].bump_texname != "")
        {
            std::string bumpPath(materials[i].bump_texname);
            std::replace(bumpPath.begin(), bumpPath.end(), '\\', '/');
            groups[i + 1].specular_path = directory + "/" + bumpPath;
        }
    }

    std::vector<std::unordered_map<Vertex, uint32_t>> uniqueVerticesPerGroup(materials.size() + 1);

    auto appendVertex = [&uniqueVerticesPerGroup, &groups](const Vertex& vertex, int material_id) {
        // 0 for unknown material
        auto& unique_vertices = uniqueVerticesPerGroup[material_id + 1];
        auto& group = groups[material_id + 1];
        if (unique_vertices.count(vertex) == 0)
        {
            unique_vertices[vertex] = group.vertices.size(); // auto incrementing size
            group.vertices.push_back(vertex);
        }
        group.indices.push_back(static_cast<unsigned int>(unique_vertices[vertex]));
    };

    for (const auto& shape : shapes)
    {

        size_t indexOffset = 0;
        for (size_t n = 0; n < shape.mesh.num_face_vertices.size(); n++)
        {
            // per face
            auto ngon = shape.mesh.num_face_vertices[n];
            auto material_id = shape.mesh.material_ids[n];
            for (size_t f = 0; f < ngon; f++)
            {
                const auto& index = shape.mesh.indices[indexOffset + f];

                Vertex vertex;

                vertex.position = {attrib.vertices[3 * index.vertex_index + 0],
                                   attrib.vertices[3 * index.vertex_index + 1],
                                   attrib.vertices[3 * index.vertex_index + 2]};

                // Check if it has texture coordinates
                if (index.texcoord_index >= 0)
                {
                    vertex.texCoords = {attrib.texcoords[2 * index.texcoord_index + 0],
                                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};
                }

                // Check if it has normals
                if (index.normal_index >= 0)
                {
                    vertex.normal = {attrib.normals[3 * index.normal_index + 0],
                                     attrib.normals[3 * index.normal_index + 1],
                                     attrib.normals[3 * index.normal_index + 2]};
                }
                appendVertex(vertex, material_id);
            }
            indexOffset += ngon;
        }
    }

    for (size_t g = 0; g < groups.size(); g++)
    {
        // Check if MeshMaterialGroup has anything
        if (groups[g].vertices.size() != 0)
        {
            std::vector<Texture> textures{};
            if (!groups[g].diffuse_path.empty())
            {

                Texture diffuseTexture(groups[g].diffuse_path.c_str());
                diffuseTexture.SetPath(groups[g].diffuse_path);
                diffuseTexture.SetType("texture_diffuse");
                textures.push_back(diffuseTexture);
            }
            if (!groups[g].specular_path.empty())
            // if (false)
            {
                Texture specularTexture(groups[g].specular_path.c_str());
                specularTexture.SetPath(groups[g].specular_path);
                specularTexture.SetType("texture_specular");
                textures.push_back(specularTexture);
            }

            meshes.push_back({groups[g].vertices, groups[g].indices, textures});
        }
    }
}
