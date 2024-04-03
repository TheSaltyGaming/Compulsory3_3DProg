#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "../core/Shader.h"

class Plane
{
public:
    unsigned int VBO, VAO, EBO;
    
    std::vector<float> planeVertices;
    Plane();

    glm::mat4 model = glm::mat4(1.0f);
    
    void CreateMeshPlane();
    void DrawPlane(unsigned int shaderProgram);

};
