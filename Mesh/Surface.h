//
// Created by Anders on 05.04.2024.
//

#ifndef SURFACE_H
#define SURFACE_H
#include <vector>
#include <glm/glm.hpp>

#include "../core/Vertex.h"


class Surface {

public:
    int size = 10;


    std::vector<Triangle> GetTriangles();
    unsigned int VBO, VAO, EBO;

    glm::vec3 worldPosition;

    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<unsigned int> indices;


    Surface();
    Surface(int size);

    glm::mat4 model = glm::mat4(1.0f);

    void Setup();

    void Draw(unsigned int shaderProgram);

    void CalculateNormals(std::vector<Triangle>& epictriaingles);
};



#endif //SURFACE_H
