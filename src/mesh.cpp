#include "mesh.h"
#include "core.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
    : vertices{vertices}, indices{indices}, textures{textures}
{
    setupMesh();
}

void Mesh::setupMesh()
{
    vao = std::make_unique<VertexArray>();
    VertexBufferLayout layout = {
        {"aPos", DataType::Float3}, {"aNormals", DataType::Float3}, {"aTexCoords", DataType::Float2}};
    vbo = std::make_unique<VertexBuffer>(layout, &vertices[0], vertices.size() * sizeof(Vertex));
    ibo = std::make_unique<IndexBuffer>((const void*)&indices[0], indices.size());

    vao->AddBuffer(*vbo);
    vao->Unbind();
}

void Mesh::Draw(Shader& shader)
{
    shader.Bind();
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for (size_t i = 0; i < textures.size(); i++)
    {
        Texture texture = textures[i];
        std::string number;
        std::string name = texture.GetType();
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else
            number = std::to_string(specularNr++);
        shader.SetUniform1i("material."+name+number, i);
        texture.Bind(i);
    }
    glActiveTexture(GL_TEXTURE0);

    vao->Bind();
    ibo->Bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    vao->Unbind();
    ibo->Unbind();
    shader.Unbind();
}
