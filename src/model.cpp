#include "model.h"

#include <algorithm>
#include <glad/gl.h>
#include <string>
#include <iostream>
#include <vector>
#define TINYOBJLOADER_IMPLEMENTATION
#include <vendor/tiny_obj_loader.h>

void draw(Model* model, const idk_mat4& transform, OpenGLProgramCommon* shader)
{
  MeshNode* mesh_node = model->meshes;
  while(mesh_node != 0)
  {
    MeshMaterialGroup* mesh = mesh_node->data;
    glUseProgram(shader->program_id);
    Texture* diffuse_tex = mesh->materials.diffuse_tex;
    Texture* specular_tex = mesh->materials.specular_tex;
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
    glBindVertexArray(mesh->vao->id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo->id);
    glDrawElements(GL_TRIANGLES, mesh->num_indices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram(0);
    mesh_node = mesh_node->next;
  }
}

Model* create_model(Arena* arena, const std::string& path)
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

  const tinyobj::attrib_t& attrib = reader.GetAttrib();
  const std::vector<tinyobj::shape_t>& shapes = reader.GetShapes();
  const std::vector<tinyobj::material_t>& materials = reader.GetMaterials();

  Arena* temp_arena = arena_alloc(Megabytes(1));
  Material * all_materials = (Material*)arena_push(temp_arena, materials.size() * sizeof(Material));
  // Load all textures
  for (size_t i = 0; i < materials.size(); i++)
  {
    if (!materials[i].diffuse_texname.empty())
    {
      // TODO(ricardo): put this into a function
      std::string diffuse_path(materials[i].diffuse_texname);
      std::replace(diffuse_path.begin(), diffuse_path.end(), '\\', '/');
      std::string diffuse_path_final = directory + "/" + diffuse_path;
      Texture* texture = opengl_create_texture(arena, diffuse_path_final.c_str(), diffuse);
      (all_materials+i)->diffuse_tex = texture;
    }
    if (!materials[i].specular_texname.empty())
    {
      std::string specular_path(materials[i].specular_texname);
      std::replace(specular_path.begin(), specular_path.end(), '\\', '/');
      std::string specular_path_final = directory + "/" + specular_path;
      Texture* texture = opengl_create_texture(arena, specular_path_final.c_str(), specular);
      (all_materials+i)->specular_tex = texture;
    }
    else if (!materials[i].bump_texname.empty())
    {
      std::string bump_path(materials[i].bump_texname);
      std::replace(bump_path.begin(), bump_path.end(), '\\', '/');
      std::string bump_path_final = directory + "/" + bump_path;
      Texture* texture = opengl_create_texture(arena, bump_path_final.c_str(), specular);
      (all_materials+i)->specular_tex = texture;
    }
  }

  Model* model = (Model*)arena_push(arena,sizeof(Model));
  model->meshes = (MeshNode*)arena_push(arena, sizeof(MeshNode));
  // Head
  MeshNode* mesh = model->meshes;

  uint32_t mesh_index = 0;
  for (size_t s = 0; s < shapes.size(); s++)
  {
    size_t index_offset = 0;
    int previous_face_material_id = -1;

    size_t num_faces = shapes[s].mesh.num_face_vertices.size();
    // Create new linked list node
    if(s != 0){
      mesh->next = (MeshNode*)arena_push(arena, sizeof(MeshNode));
      mesh = mesh->next;
    }
    mesh->data = (MeshMaterialGroup*)arena_push(arena, sizeof(MeshMaterialGroup));

    // NOTE(ricardo): we assume that the mesh is triangulated (only 3 vertices per face)
    mesh->data->vertices = (Vertex*)arena_push(arena, sizeof(Vertex) * num_faces*3);
    mesh->data->indices = (uint32_t*)arena_push(arena, sizeof(unsigned int) * num_faces*3);
    uint32_t indice = 0;
    for (size_t f = 0; f < num_faces; f++)
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

        // NOTE(ricardo): if the face_material_id is different we want to make it
        // into another mesh
        int face_material_id = shapes[s].mesh.material_ids[f];
        if(previous_face_material_id != -1 &&
            face_material_id != previous_face_material_id)
        {
          uint32_t model_num_vertices = mesh->data->num_vertices;
          Vertex* newPtrV = mesh->data->vertices + model_num_vertices;
          uint32_t model_num_indices = mesh->data->num_indices;
          uint32_t* newPtrI = mesh->data->indices + model_num_indices;
          mesh_index++;
          previous_face_material_id = shapes[s].mesh.material_ids[f];

          mesh->next = (MeshNode*)arena_push(arena, sizeof(MeshNode));
          mesh = mesh->next;
          mesh->data = (MeshMaterialGroup*)arena_push(arena, sizeof(MeshMaterialGroup));
          mesh->data->vertices = newPtrV;
          mesh->data->indices = newPtrI;
          indice = 0;
        }
        uint32_t model_num_vertices = mesh->data->num_vertices++;
        *(mesh->data->vertices + model_num_vertices) = vertex;
        uint32_t model_num_indices = mesh->data->num_indices++;
        // TODO(ricardo): Not doing indices yet, should we do it?
        // there some duplicate vertex data but this would mean to
        // create a hash map for each vertex and store its index, slower?
        *(mesh->data->indices +  model_num_indices) = indice;
        mesh->data->materials.diffuse_tex = all_materials[face_material_id].diffuse_tex;
        mesh->data->materials.specular_tex = all_materials[face_material_id].specular_tex;
        indice++;
      }
      index_offset+=fv;
      previous_face_material_id = shapes[s].mesh.material_ids[f];
    }
    mesh_index++;
  }

  // Create OpenGL objects
  MeshNode* mesh_node = model->meshes;
  while(mesh_node != 0)
  {
    MeshMaterialGroup* mesh = mesh_node->data;
    if (mesh->num_vertices != 0 )
    {
      mesh->vao = opengl_create_vertex_array(arena);
      mesh->vbo = opengl_create_vertex_buffer(arena, mesh->vertices, mesh->num_vertices * sizeof(Vertex));
      mesh->ibo = opengl_create_index_buffer(arena, (const void*)(mesh->indices), mesh->num_indices);
      int enabled_attribs = 0;
      int stride = 32;
      int offset = 0;
      // Position
      opengl_add_element_to_layout(DataType::Float3, false, &enabled_attribs, stride, &offset, mesh->vao,
          mesh->vbo);
      // Normals
      opengl_add_element_to_layout(DataType::Float3, false, &enabled_attribs, stride, &offset, mesh->vao,
          mesh->vbo);
      // Texture Coords
      opengl_add_element_to_layout(DataType::Float2, false, &enabled_attribs, stride, &offset, mesh->vao,
          mesh->vbo);
    }
    mesh_node = mesh_node->next;
  }
  return model;
}
