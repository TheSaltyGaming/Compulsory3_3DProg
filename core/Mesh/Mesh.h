//
// Created by Ander on 18.03.2024.
//

#ifndef MESH_H
#define MESH_H
#include <vector>

#include "Texture.h"
#include "../Shader.h"
#include "../Vertex.h"


class Mesh {

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

    void draw(Shader &shader);

private:
    unsigned int VAO, VBO, EBO;

    void SetupMesh();
};



#endif //MESH_H
