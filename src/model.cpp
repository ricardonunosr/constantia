#include "model.h"

#include "core.h"
#include <algorithm>
#include <unordered_map>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

void Model::Draw(Shader& shader)
{
    for (int i = 1; i < meshes.size(); i++)
    {
        auto& mesh = meshes[i];
        shader.Bind();
        for (uint32_t k = 0; k < mesh.textures.size(); k++)
        {
            Texture& texture = mesh.textures[k];
            std::string name = texture.GetType();
            shader.SetUniform1i("material." + name, k);
            texture.Bind(k);
        }
        glActiveTexture(GL_TEXTURE0);

        mesh.vao->Bind();
        mesh.ibo->Bind();
        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
        mesh.vao->Unbind();
        mesh.ibo->Unbind();
        Shader::Unbind();
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
    meshes.resize(materials.size() + 1);

    for (size_t i = 0; i < materials.size(); i++)
    {
        auto& mesh = meshes[i + 1];
        if (!materials[i].diffuse_texname.empty())
        {
            std::string diffusePath(materials[i].diffuse_texname);
            std::replace(diffusePath.begin(), diffusePath.end(), '\\', '/');
            std::string diffuse_path_final = directory + "/" + diffusePath;
            mesh.textures.emplace_back(diffuse_path_final.c_str(), "texture_diffuse");
        }
        if (!materials[i].specular_texname.empty())
        {
            std::string specularPath(materials[i].specular_texname);
            std::replace(specularPath.begin(), specularPath.end(), '\\', '/');
            std::string specular_path_final = directory + "/" + specularPath;
            mesh.textures.emplace_back(specular_path_final.c_str(), "texture_specular");
        }
        else if (!materials[i].bump_texname.empty())
        {
            std::string bumpPath(materials[i].bump_texname);
            std::replace(bumpPath.begin(), bumpPath.end(), '\\', '/');
            std::string bump_path_final = directory + "/" + bumpPath;
            mesh.textures.emplace_back(bump_path_final.c_str(), "texture_specular");
        }
    }

    std::vector<std::unordered_map<Vertex, uint32_t>> uniqueVerticesPerGroup(materials.size() + 1);
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

                Vertex vertex{};

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

                // 0 for unknown material
                auto& unique_vertices = uniqueVerticesPerGroup[material_id + 1];
                auto& group = meshes[material_id + 1];
                if (unique_vertices.count(vertex) == 0)
                {
                    unique_vertices[vertex] = group.vertices.size(); // auto incrementing size
                    group.vertices.push_back(vertex);
                }
                group.indices.push_back(static_cast<unsigned int>(unique_vertices[vertex]));
            }
            indexOffset += ngon;
        }
    }

    for (auto& mesh : meshes)
    {
        // Check if MeshMaterialGroup has anything
        if (!mesh.vertices.empty())
        {
            mesh.vao = std::make_unique<VertexArray>();
            VertexBufferLayout layout = {
                {"aPos", DataType::Float3}, {"aNormals", DataType::Float3}, {"aTexCoords", DataType::Float2}};
            mesh.vbo = std::make_unique<VertexBuffer>(layout, &mesh.vertices[0], mesh.vertices.size() * sizeof(Vertex));
            mesh.ibo = std::make_unique<IndexBuffer>((const void*)&mesh.indices[0], mesh.indices.size());
            mesh.vao->AddBuffer(*mesh.vbo);
            mesh.vao->Unbind();
        }
    }
}
