//
// Created by Ander on 08.04.2024.
//

#ifndef LINE_H
#define LINE_H
#include <vector>
#include <glm/vec3.hpp>


class Line {

public:
        unsigned VAO, VBO, EBO;
        Line();

        Line(std::vector<glm::vec3> points);

        void Setup();

        void Draw();

        std::vector<glm::vec3> linePoints;;

};



#endif //LINE_H
