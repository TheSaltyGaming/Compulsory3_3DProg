#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Vertex
{

    float x,y,z,r,g,b;
    Vertex() = default;

    Vertex(glm::vec3 position) {
        Position = position;
        Normal = glm::vec3(0.f);
        TexCoords = glm::vec2(0.f);
    }

    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Triangle {
    glm::vec3 v0, v1, v2, normal;

    Triangle(const glm::vec3& vertex0, const glm::vec3& vertex1, const glm::vec3& vertex2, const glm::vec3& normal1)
    : v0(vertex0), v1(vertex1), v2(vertex2), normal(normal1) {}

    Triangle() : v0(0.0f), v1(0.0f), v2(0.0f) {}
};
