#include "Box.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

Box::Box()
{
    
}


Box::Box(float minX, float minY, float minZ, float maxX, float maxY, float maxZ, box_type boxtype)
{
    type = boxtype;
    SetColor();
    boxVertices = {
        // positions
        minX,  maxY,  minZ, red, green, blue,
        minX,  minY,  minZ, red, green, blue,
        maxX,  minY,  minZ, red, green, blue,
        maxX,  maxY,  minZ, red, green, blue,
        minX,  maxY,  maxZ, red, green, blue,
        minX,  minY,  maxZ, red, green, blue,
        maxX,  minY,  maxZ, red, green, blue,
        maxX,  maxY,  maxZ, red, green, blue
    };
    Setup();
}

Box::Box(float radius, box_type boxtype)
{
    // set box type
    type = boxtype;
    SetColor();
    minX = -radius;
    minY = -radius;
    minZ = -radius;
    maxX = radius;
    maxY = radius;
    maxZ = radius;
    boxVertices = {
        // positions
        minX,  maxY,  minZ,  red, green, blue,
        minX,  minY,  minZ,  red, green, blue,
        maxX,  minY,  minZ,  red, green, blue,
        maxX,  maxY,  minZ,  red, green, blue,
        minX,  maxY,  maxZ,  red, green, blue,
        minX,  minY,  maxZ,  red, green, blue,
        maxX,  minY,  maxZ,  red, green, blue,
        maxX,  maxY,  maxZ,  red, green, blue
    };
    Setup();
}

void Box::SetColor()
{
    switch (type)
    {
    case Door:
        red = 0.5f;
        green = 0.0f;
        blue = 1.f;
        break;
    case House:
        red = 0.08f;
        green = 0.39f;
        blue = 0.25f;
        break;
    case Pickup:
        red = 0.0f;
        green = 1.f;
        blue = 0.5f;
        break;
    case Player:
        red = 1.f;
        green = 0.0f;
        blue = 0.0f;
        break;
    case Npc:
        red = 1.f;
        green = 1.f;
        blue = 0.0f;
        break;
    default:
        break;
    }
}

void Box::Setup()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    //glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, planeVertices.size() * sizeof(float), planeVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, boxVertices.size() * sizeof(float), boxVertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // Unbind VAO and VBO
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);
}

void Box::Draw(unsigned int shaderProgram)
{
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    
    glBindVertexArray(VAO);
    //glDrawArrays(GL_TRIANGLES, 0, (width - 1) * (length - 1) * 6);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

glm::vec3 Box::GetMin() {
    glm::vec4 worldMin = model * glm::vec4(minX, minY, minZ, 1.0f);
    return glm::vec3(worldMin);
}

glm::vec3 Box::GetMax() {
    glm::vec4 worldMax = model * glm::vec4(maxX, maxY, maxZ, 1.0f);
    return glm::vec3(worldMax);
}

bool Box::CheckCollision(Box* otherbox)
{
    glm::vec3 thisMin = this->GetMin();
    glm::vec3 thisMax = this->GetMax();
    glm::vec3 otherMin = otherbox->GetMin();
    glm::vec3 otherMax = otherbox->GetMax();

    // Check for overlap in the x, y, and z dimensions
    bool overlapInX = thisMax.x >= otherMin.x && thisMin.x <= otherMax.x;
    bool overlapInY = thisMax.y >= otherMin.y && thisMin.y <= otherMax.y;
    bool overlapInZ = thisMax.z >= otherMin.z && thisMin.z <= otherMax.z;

    // If there is overlap in all three dimensions, the boxes are colliding
    return overlapInX && overlapInY && overlapInZ;
}
