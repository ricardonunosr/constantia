#include "model.h"

#include <algorithm>
#include <glad/gl.h>
#include <spdlog/spdlog.h>
#include <unordered_map>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

void Model::draw(const Frustum& frustum, const glm::mat4& transform, OpenGLProgramCommon* shader, unsigned int& display,
                 unsigned int& total)
{
    // Note(ricardo): We start to draw at index 1 because 0 is a no texture mesh
    for (int mesh_index = 1; mesh_index < (int)m_meshes.size(); mesh_index++)
    {
        //        if (m_bounding_volume->is_on_frustum(frustum, transform))
        //        {
        auto& mesh = m_meshes[mesh_index];
        glUseProgram(shader->program_id);
        for (uint32_t k = 0; k < mesh.textures.size(); k++)
        {
            Texture* texture = mesh.textures[k];
            if (texture->type == diffuse)
            {
                glUniform1i(shader->material_texture_diffuse, k);
            }
            else if (texture->type == specular)
            {
                glUniform1i(shader->material_texture_specular, k);
            }
            opengl_bind_texture(texture->id, k);
        }
        glActiveTexture(GL_TEXTURE0);

        mesh.vao->bind();
        mesh.ibo->bind();
        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
        mesh.vao->unbind();
        mesh.ibo->unbind();
        glUseProgram(0);
        display++;
        //        }
        total++;
    }
}

namespace std
{
template <> struct hash<Vertex>
{
    size_t operator()(Vertex const& vertex) const
    {
        return ((hash<glm::vec3>()(vertex.position))) ^ (hash<glm::vec2>()(vertex.tex_coords) << 1);
    }
};
} // namespace std

void Model::load_model(const std::string& path)
{
    {
        m_directory = path.substr(0, path.find_last_of('/'));

        tinyobj::ObjReaderConfig reader_config;
        reader_config.mtl_search_path = m_directory;
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

        const auto& attrib = reader.GetAttrib();
        const auto& shapes = reader.GetShapes();
        const auto& materials = reader.GetMaterials();

        /*  We are grouping every mesh that was the same material.
         *  This way we can group every mesh into one draw call instead of multiple for the same mesh.
         *  +1 for an unknown material (index 0 is the unknown material)
         */
        m_meshes.resize(materials.size() + 1);

        for (size_t i = 0; i < materials.size(); i++)
        {
            auto& mesh = m_meshes[i + 1];
            if (!materials[i].diffuse_texname.empty())
            {
                std::string diffuse_path(materials[i].diffuse_texname);
                std::replace(diffuse_path.begin(), diffuse_path.end(), '\\', '/');
                std::string diffuse_path_final = m_directory + "/" + diffuse_path;
                Texture* texture = (Texture*)malloc(sizeof(Texture));
                opengl_create_texture(diffuse_path_final.c_str(), diffuse, texture);
                mesh.textures.push_back(texture);
            }
            if (!materials[i].specular_texname.empty())
            {
                std::string specular_path(materials[i].specular_texname);
                std::replace(specular_path.begin(), specular_path.end(), '\\', '/');
                std::string specular_path_final = m_directory + "/" + specular_path;
                Texture* texture = (Texture*)malloc(sizeof(Texture));
                opengl_create_texture(specular_path_final.c_str(), specular, texture);
                mesh.textures.push_back(texture);
            }
            else if (!materials[i].bump_texname.empty())
            {
                std::string bump_path(materials[i].bump_texname);
                std::replace(bump_path.begin(), bump_path.end(), '\\', '/');
                std::string bump_path_final = m_directory + "/" + bump_path;
                Texture* texture = (Texture*)malloc(sizeof(Texture));
                opengl_create_texture(bump_path_final.c_str(), specular, texture);
                mesh.textures.push_back(texture);
            }
        }

        std::vector<std::unordered_map<Vertex, uint32_t>> unique_vertices_per_group(materials.size() + 1);
        for (const auto& shape : shapes)
        {
            size_t index_offset = 0;
            for (size_t n = 0; n < shape.mesh.num_face_vertices.size(); n++)
            {
                // per face
                auto ngon = shape.mesh.num_face_vertices[n];
                auto material_id = shape.mesh.material_ids[n];
                for (size_t f = 0; f < ngon; f++)
                {
                    const auto& index = shape.mesh.indices[index_offset + f];

                    Vertex vertex{};

                    vertex.position = {attrib.vertices[3 * index.vertex_index + 0],
                                       attrib.vertices[3 * index.vertex_index + 1],
                                       attrib.vertices[3 * index.vertex_index + 2]};

                    // Check if it has texture coordinates
                    if (index.texcoord_index >= 0)
                    {
                        vertex.tex_coords = {attrib.texcoords[2 * index.texcoord_index + 0],
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
                    auto& unique_vertices = unique_vertices_per_group[material_id + 1];
                    auto& group = m_meshes[material_id + 1];
                    if (unique_vertices.count(vertex) == 0)
                    {
                        unique_vertices[vertex] = group.vertices.size(); // auto incrementing size
                        group.vertices.push_back(vertex);
                    }
                    group.indices.push_back(static_cast<unsigned int>(unique_vertices[vertex]));
                }
                index_offset += ngon;
            }
        }

        for (auto& mesh : m_meshes)
        {
            // Check if MeshMaterialGroup has anything
            if (!mesh.vertices.empty())
            {
                mesh.vao = std::make_unique<VertexArray>();
                VertexBufferLayout layout = {
                    {"aPos", DataType::Float3}, {"aNormals", DataType::Float3}, {"aTexCoords", DataType::Float2}};
                mesh.vbo = std::make_unique<VertexBuffer>(layout, (mesh.vertices).data(),
                                                          mesh.vertices.size() * sizeof(Vertex));
                mesh.ibo = std::make_unique<IndexBuffer>((const void*)(mesh.indices).data(), mesh.indices.size());
                mesh.vao->add_buffer(*mesh.vbo);
                mesh.vao->unbind();
            }
        }
    }
}