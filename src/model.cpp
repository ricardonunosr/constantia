#include "model.h"

#include <algorithm>
#include <glad/gl.h>
#include <unordered_map>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#define TINYOBJLOADER_IMPLEMENTATION
#include <vendor/tiny_obj_loader.h>

void draw(Model* model, const idk_mat4& transform, OpenGLProgramCommon* shader)
{
    for (int mesh_index = 1; mesh_index < (int)model->meshes.size(); mesh_index++)
    {
        auto& mesh = model->meshes[mesh_index];
        glUseProgram(shader->program_id);
        for (uint32_t k = 0; k < 2; k++)
        {
            if(mesh.textures.size() > 0)
            {
                Texture* texture = mesh.textures[k];
                if(texture){
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
            }
        }
        glActiveTexture(GL_TEXTURE0);

        glBindVertexArray(mesh.vao->id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo->id);
        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glUseProgram(0);
    }
}

static std::vector<int> find_unique_elements(std::vector<int> vector)
{
    std::vector<int> result;
    for(int vector_index = 0; vector_index< vector.size(); vector_index++)
    {
        if(result.size() != 0)
        {
            for(int result_index = 0; result_index < result.size(); result_index++)
            {
               if(result[result_index] != vector[vector_index])
               {
                   result.push_back(vector[vector_index]);
               }
            }
        }
        else
        {
            result.push_back(vector[vector_index]);
        }
    }

    return result;
}

void create_model(Model* model, const std::string& path)
{
        std::string directory =path.substr(0, path.find_last_of('/'));  

        tinyobj::ObjReaderConfig reader_config;
        reader_config.mtl_search_path = directory;
        tinyobj::ObjReader reader;

        if (!reader.ParseFromFile(path, reader_config))
        {
            if (!reader.Error().empty())
            {
                printf("TinyObjReader: %s", reader.Error().c_str());
            }
            exit(1);
        }

        if (!reader.Warning().empty())
        {
            printf("TinyObjReader: %s", reader.Warning().c_str());
        }

        const auto& attrib = reader.GetAttrib();
        const auto& shapes = reader.GetShapes();
        const auto& materials = reader.GetMaterials();

        // NOTE(ricardo): material = diffuse + specular textures
        struct Material
        {
            Texture* diffuse_tex;
            Texture* specular_tex;
        };
        std::vector<Material> all_materials(materials.size());
        // Load all textures
        for (size_t i = 0; i < materials.size(); i++)
        {
            if (!materials[i].diffuse_texname.empty())
            {
                std::string diffuse_path(materials[i].diffuse_texname);
                std::replace(diffuse_path.begin(), diffuse_path.end(), '\\', '/');
                std::string diffuse_path_final = directory + "/" + diffuse_path;
                // Texture* texture = (Texture*)malloc(sizeof(Texture));
                 Texture* texture = (Texture*)new Texture();
                opengl_create_texture(diffuse_path_final.c_str(), diffuse, texture);
                all_materials[i].diffuse_tex = texture;
            }
            if (!materials[i].specular_texname.empty())
            {
                std::string specular_path(materials[i].specular_texname);
                std::replace(specular_path.begin(), specular_path.end(), '\\', '/');
                std::string specular_path_final = directory + "/" + specular_path;
                //Texture* texture = (Texture*)malloc(sizeof(Texture));
                Texture* texture = (Texture*)new Texture();
                opengl_create_texture(specular_path_final.c_str(), specular, texture);
                all_materials[i].specular_tex = texture;
            }
            else if (!materials[i].bump_texname.empty())
            {
                std::string bump_path(materials[i].bump_texname);
                std::replace(bump_path.begin(), bump_path.end(), '\\', '/');
                std::string bump_path_final = directory + "/" + bump_path;
                //Texture* texture = (Texture*)malloc(sizeof(Texture));
                Texture* texture = (Texture*)new Texture();
                opengl_create_texture(bump_path_final.c_str(), specular, texture);
                all_materials[i].specular_tex = texture;
            }
        }

        model->meshes.resize(shapes.size());
        for (size_t shape_index = 0; shape_index < shapes.size(); shape_index++)
        {
            auto& shape = shapes[shape_index];
            for (size_t indice = 0; indice < shape.mesh.indices.size(); indice++)
            {
                auto& index = shape.mesh.indices[indice];
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

                model->meshes[shape_index].vertices.push_back(vertex);
                std::vector<unsigned int>& indices = model->meshes[shape_index].indices;
                indices.push_back(indices.size());
            }
            std::vector<int> material_ids = shape.mesh.material_ids;
            std::sort(material_ids.begin(),material_ids.end());
            material_ids.erase(unique(material_ids.begin(),material_ids.end()),material_ids.end());

            for(int unique_mat_index = 0; unique_mat_index < material_ids.size(); unique_mat_index++)
            {
                int unique_index = material_ids[unique_mat_index];
                model->meshes[shape_index].textures.push_back(all_materials[unique_index].diffuse_tex);
                model->meshes[shape_index].textures.push_back(all_materials[unique_index].specular_tex);
            }
        }

        // Create OpenGL objects
        for (auto& mesh : model->meshes)
        {
            if (!mesh.vertices.empty())
            {
                mesh.vao = (VertexArray*)malloc(sizeof(VertexArray));
                opengl_create_vertex_array(mesh.vao);
                mesh.vbo = (VertexBuffer*)malloc(sizeof(VertexBuffer));
                opengl_create_vertex_buffer((mesh.vertices).data(), mesh.vertices.size() * sizeof(Vertex), mesh.vbo);
                mesh.ibo = (IndexBuffer*)malloc(sizeof(IndexBuffer));
                opengl_create_index_buffer((const void*)(mesh.indices).data(), mesh.indices.size(), mesh.ibo);
                int enabled_attribs = 0;
                int stride = 32;
                int offset = 0;
                // Position
                opengl_add_element_to_layout(DataType::Float3, false, &enabled_attribs, stride, &offset, mesh.vao,
                                             mesh.vbo);
                // Normals
                opengl_add_element_to_layout(DataType::Float3, false, &enabled_attribs, stride, &offset, mesh.vao,
                                             mesh.vbo);
                // Texture Coords
                opengl_add_element_to_layout(DataType::Float2, false, &enabled_attribs, stride, &offset, mesh.vao,
                                             mesh.vbo);
            }
        }
    }
