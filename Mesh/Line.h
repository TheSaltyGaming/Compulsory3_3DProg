//
// Created by Ander on 08.04.2024.
//

#ifndef LINE_H
#define LINE_H
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Line {

public:
        unsigned VAO, VBO, EBO;
        Line();

        Line(std::vector<glm::vec3> points);

        void Setup();

        void Draw(unsigned int shaderProgram);

        std::vector<glm::vec3> linePoints;;

        glm::mat4 model = glm::mat4(1.0f);


        void SetPosition(glm::vec3 position) {
                model = glm::translate(glm::mat4(1.0f), position);
        }
};



#endif //LINE_H
