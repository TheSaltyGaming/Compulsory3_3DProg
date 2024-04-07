#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Vertex
{
    float x, y, z, r, g, b;

    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Triangle {
    glm::vec3 v0, v1, v2;

    Triangle(const glm::vec3& vertex0, const glm::vec3& vertex1, const glm::vec3& vertex2)
    : v0(vertex0), v1(vertex1), v2(vertex2) {}

    Triangle() : v0(0.0f), v1(0.0f), v2(0.0f) {}
};
