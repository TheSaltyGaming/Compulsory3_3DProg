//
// Created by Anders on 05.04.2024.
//

#include "Surface.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>


std::vector<Triangle> Surface::GetTriangles() const {
    std::vector<Triangle> triangles;
    for(size_t i = 0; i < indices.size(); i += 3)
    {
        glm::vec3 v0 = glm::vec3(vertices[indices[i] * 3], vertices[indices[i] * 3 + 1], vertices[indices[i] * 3 + 2]);
        glm::vec3 v1 = glm::vec3(vertices[indices[i + 1] * 3], vertices[indices[i + 1] * 3 + 1], vertices[indices[i + 1] * 3 + 2]);
        glm::vec3 v2 = glm::vec3(vertices[indices[i + 2] * 3], vertices[indices[i + 2] * 3 + 1], vertices[indices[i + 2] * 3 + 2]);

        triangles.emplace_back(v0, v1, v2);
    }
    return triangles;
}

Surface::Surface() {
}

Surface::Surface(int sizeint) {

    int detail = 10;
    size = sizeint * detail;

    // Generate vertices for the grid
    for (int i = 0; i <= size; ++i) {
        for (int j = 0; j <= size; ++j) {
            vertices.push_back((float)i / detail);
            vertices.push_back(0.6*sin((float)i / detail));
            vertices.push_back((float)j / detail);

            normals.push_back(0.0f);
            normals.push_back(1.0f);
            normals.push_back(0.0f);
        }
    }

    // Generate indices for the grid
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            int topLeft = i * (size + 1) + j;
            int bottomLeft = (i + 1) * (size + 1) + j;

            // First triangle

            indices.push_back(bottomLeft);
            indices.push_back(bottomLeft + 1);
            indices.push_back(topLeft + 1);

            // Second triangle
            indices.push_back(topLeft+1);
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
        }
    }

    Setup();
}

void Surface::Setup() {

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);


    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);




    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glBindVertexArray(0);

}

void Surface::Draw(unsigned int shaderProgram) {

    int modelLoc = glGetUniformLocation(shaderProgram, "model");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(VAO);
    //glDrawArrays(GL_TRIANGLES, 0, (width - 1) * (length - 1) * 6);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
