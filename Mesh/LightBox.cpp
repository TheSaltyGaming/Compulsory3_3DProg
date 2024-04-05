//
// Created by Anders on 05.04.2024.
//

#include "LightBox.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

LightBox::LightBox() {
}

LightBox::LightBox(float radius) {

    // set box type
    minX = -radius;
    minY = -radius;
    minZ = -radius;
    maxX = radius;
    maxY = radius;
    maxZ = radius;
    boxVertices = {
        // positions
        minX,  maxY,  minZ,
        minX,  minY,  minZ,
        maxX,  minY,  minZ,
        maxX,  maxY,  minZ,
        minX,  maxY,  maxZ,
        minX,  minY,  maxZ,
        maxX,  minY,  maxZ,
        maxX,  maxY,  maxZ
    };
    Setup();
}

void LightBox::Setup() {

    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    // we only need to bind to the VBO, the container's VBO's data already contains the data.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // set the vertex attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);


    glBufferData(GL_ARRAY_BUFFER, boxVertices.size() * sizeof(float), boxVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);



    // Unbind VAO and VBO
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);
}

void LightBox::Draw(unsigned int shaderProgram)
{
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));


    glBindVertexArray(lightVAO);
    //glDrawArrays(GL_TRIANGLES, 0, (width - 1) * (length - 1) * 6);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
