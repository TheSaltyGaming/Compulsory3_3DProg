//
// Created by Anders on 05.04.2024.
//

#ifndef SURFACE_H
#define SURFACE_H
#include <vector>
#include <glm/glm.hpp>


class Surface {

    int size = 10;

    unsigned int VBO, VAO, EBO;

    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<unsigned int> indices;

public:
    Surface();
    Surface(int size);

    glm::mat4 model = glm::mat4(1.0f);

    void Setup();

    void Draw(unsigned int shaderProgram);
};



#endif //SURFACE_H
