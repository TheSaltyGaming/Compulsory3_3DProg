//
// Created by Anders on 05.04.2024.
//

#ifndef LIGHTBOX_H
#define LIGHTBOX_H
#include <vector>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



class LightBox {

public:
    LightBox();
    LightBox(float radius);

    float minX, minY, minZ, maxX, maxY, maxZ;


    std::vector<float> boxVertices;

    unsigned int indices[36] = {
        0, 1, 3,
        1, 2, 3,
        4, 5, 7,
        5, 6, 7,
        0, 1, 5,
        0, 5, 4,
        3, 2, 6,
        3, 6, 7,
        0, 3, 7,
        0, 7, 4,
        1, 2, 6,
        1, 6, 5
    };
    unsigned int VBO, lightVAO, EBO;

    glm::mat4 model = glm::mat4(1.0f);

    //setup
    void Setup();


    void Draw(unsigned int shaderProgram);

    glm::vec3 GetMin();
    glm::vec3 GetMax();

};



#endif //LIGHTBOX_H
