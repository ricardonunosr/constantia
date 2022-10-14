#include "model.h"

#include <algorithm>
#include <glad/gl.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <vendor/tiny_obj_loader.h>

void draw(Model* model, const idk_mat4& transform, OpenGLProgramCommon* shader)
{
    for (int mesh_index = 0; mesh_index < (int)model->meshes.size(); mesh_index++)
    {
        auto& mesh = model->meshes[mesh_index];
        glUseProgram(shader->program_id);
        Texture* diffuse_tex = mesh.materials.diffuse_tex;
        Texture* specular_tex = mesh.materials.specular_tex;
        if(diffuse_tex)
        {
            glUniform1i(shader->material_texture_diffuse, 0);
            opengl_bind_texture(diffuse_tex->id, 0);
        }
        if(specular_tex)
        {
            glUniform1i(shader->material_texture_specular, 1);
            opengl_bind_texture(specular_tex->id, 1);
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

        tinyobj::attrib_t attrib = reader.GetAttrib();
        std::vector<tinyobj::shape_t> shapes = reader.GetShapes();
        std::vector<tinyobj::material_t> materials = reader.GetMaterials();

        std::vector<Material> all_materials(materials.size());
        // Load all textures
        for (size_t i = 0; i < materials.size(); i++)
        {
            if (!materials[i].diffuse_texname.empty())
            {
                // TODO(ricardo): put this into a function
                std::string diffuse_path(materials[i].diffuse_texname);
                std::replace(diffuse_path.begin(), diffuse_path.end(), '\\', '/');
                std::string diffuse_path_final = directory + "/" + diffuse_path;
                 Texture* texture = (Texture*)new Texture();
                opengl_create_texture(diffuse_path_final.c_str(), diffuse, texture);
                all_materials[i].diffuse_tex = texture;
            }
            if (!materials[i].specular_texname.empty())
            {
                std::string specular_path(materials[i].specular_texname);
                std::replace(specular_path.begin(), specular_path.end(), '\\', '/');
                std::string specular_path_final = directory + "/" + specular_path;
                Texture* texture = (Texture*)new Texture();
                opengl_create_texture(specular_path_final.c_str(), specular, texture);
                all_materials[i].specular_tex = texture;
            }
            else if (!materials[i].bump_texname.empty())
            {
                std::string bump_path(materials[i].bump_texname);
                std::replace(bump_path.begin(), bump_path.end(), '\\', '/');
                std::string bump_path_final = directory + "/" + bump_path;
                Texture* texture = (Texture*)new Texture();
                opengl_create_texture(bump_path_final.c_str(), specular, texture);
                all_materials[i].specular_tex = texture;
            }
        }

        model->meshes.resize(shapes.size());
        uint32_t mesh_index = 0;
        for (size_t s = 0; s < shapes.size(); s++)
        {
            size_t index_offset = 0;
            int previous_face_material_id = -1;
            for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
            {
                size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
                for(size_t v = 0; v < fv; v++)
                {
                    tinyobj::index_t index = shapes[s].mesh.indices[index_offset + v];
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

                    // NOTE(ricardo): if the face_material_id is different we want to make it into another mesh
                    int face_material_id = shapes[s].mesh.material_ids[f];
                    if(previous_face_material_id != -1 &&
                            face_material_id != previous_face_material_id)
                    {
                        mesh_index++;
                        previous_face_material_id = shapes[s].mesh.material_ids[f];
                        model->meshes.resize(model->meshes.size() + 1);
                    }

                    model->meshes[mesh_index].vertices.push_back(vertex);
                    std::vector<unsigned int>& indices = model->meshes[mesh_index].indices;
                    indices.push_back(indices.size());
                    model->meshes[mesh_index].materials.diffuse_tex = all_materials[face_material_id].diffuse_tex;
                    model->meshes[mesh_index].materials.specular_tex = all_materials[face_material_id].specular_tex;

                }
                index_offset+=fv;
                previous_face_material_id = shapes[s].mesh.material_ids[f];
            }
            mesh_index++;
        }

        // Create OpenGL objects
        for (auto& mesh : model->meshes)
        {
            if (!mesh.vertices.empty())
            {
                mesh.vao = (VertexArray*)malloc(sizeof(VertexArray));
                opengl_create_vertex_array(mesh.vao);
                mesh.vbo = (VertexBuffer*)malloc(sizeof(VertexBuffer));
                opengl_create_vertex_buffer(mesh.vertices.data(), mesh.vertices.size() * sizeof(Vertex), mesh.vbo);
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
