#ifndef __MESH_H__
#define __MESH_H__

#include <string>
#include <vector>

#include "glm.hpp"
#include <glad.h>

#include "shader.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
public:
    Mesh(std::vector<Vertex> vertices,
         std::vector<unsigned int> indices,
         std::vector<Texture> textures) noexcept :
         _vertices(vertices),
         _indices(indices),
         _textures(textures),
         VAO(0),
         VBO(0),
         EBO(0) {
        setupMesh();
    }

    void Draw(const Shader& shader) const {
        unsigned int diffuse_counter = 1;
        unsigned int specular_counter = 1;

        for (size_t i = 0; i < _textures.size(); i++) {
            const auto& texture = _textures[i];

            glActiveTexture(GL_TEXTURE0 + i);

            std::string shaderVariableName;
            if (texture.type == "texture_diffuse") {
                shaderVariableName = texture.type + std::to_string(diffuse_counter);
                diffuse_counter += 1;
            } else if (texture.type == "texture_specular") {
                shaderVariableName = texture.type + std::to_string(specular_counter);
                specular_counter += 1;
            }

            shader.setInt(shaderVariableName, i);
            glBindTexture(GL_TEXTURE_2D, texture.id);
        }

        glActiveTexture(GL_TEXTURE0);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    std::vector<Vertex> _vertices;
    std::vector<unsigned int> _indices;
    std::vector<Texture> _textures;

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    void setupMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex),
            &_vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int),
            &_indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            reinterpret_cast<void*>(0));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            reinterpret_cast<void*>(offsetof(Vertex, Normal)));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
            reinterpret_cast<void*>(offsetof(Vertex, TexCoords)));

        glBindVertexArray(0);
    }
};

#endif  // __MESH_H__
