#pragma once
#include <vector>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum box_type {Door, House, Pickup, Player, Npc};

class Box
{
public:
    Box();
    Box(float minX, float minY, float minZ, float maxX, float maxY, float maxZ, box_type boxtype);
    Box(float radius, box_type boxtype);

    void SetColor();
    float minX, minY, minZ, maxX, maxY, maxZ;

    float red = 0.08f;
    float green = 0.39f;
    float blue = 0.25f;

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
    unsigned int VBO, VAO, EBO;

    glm::mat4 model = glm::mat4(1.0f);

    //setup
    void Setup();
    
    
    void Draw(unsigned int shaderProgram);

    glm::vec3 GetMin();
    glm::vec3 GetMax();

    bool CheckCollision(Box* otherbox);

    box_type type;
    
    
};
