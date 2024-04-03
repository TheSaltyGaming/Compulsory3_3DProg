#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "../core/Vertex.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>


class NPC
{
public:
    glm::mat4 Matrix = glm::mat4(1.0,1.0,1.0,1.0,
                                 125.0/8.0,25.0/4.0,5.0/2.0,1.0,
                                 64.0,16.0,4.0,1.0,
                                 125.0,25.0,5.0,1.0);

    glm ::vec4 vector = glm::vec4(2.0, 1.0/2.0, 0.0, 1.0);

    glm::mat4 InverseMatrix = glm::inverse(Matrix);

    glm::vec4 Position =  vector*InverseMatrix;

    glm::mat4 model = glm::mat4(1.0f);
    
    float f(float x);
    void CreateLine();
    void DrawLine(unsigned int shaderProgram);
    void CreateNPC();
    glm::mat4 CalculateModelMatrix();
    void MoveNPC( glm::vec3 pos);
    void DrawNPC(unsigned int shaderProgram);

    glm::vec3 CurrentNPCPosition; 
    
    
    unsigned int VBO, VAO, EBO;
    
    std::vector<Vertex> NPCPoints();
};
