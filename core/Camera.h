#pragma once
#include <../includes/glm/fwd.hpp>
#include <../includes/glm/geometric.hpp>
#include <../includes/glm/vec3.hpp>
#include <../includes/glm/ext/matrix_clip_space.hpp>

class Camera
{
public:

    glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

    glm::vec3 direction;

    float yaw = -90.0f;
    float pitch = 0.f;
    float roll = 0.f;
    void tick();


    
};
