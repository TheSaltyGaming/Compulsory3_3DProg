
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <windows.h>
#include <limits>

#include "Line.h"
#include "Surface.h"
#include "core/Camera.h"
#include "core/FileManager.h"
#include "core/Shader.h"
#include "Mesh/Box.h"

#pragma region Public Variables

Camera MainCamera;
FileManager fileManager;
Shader shader;
Shader lightShader;


Box PlayerCollision;

Box epicCube;
Box epicCube2;

Box epicNPC;

 Line EpicLine;

Box lightbox;

Surface surface;

bool firstMouse = true; // Used in mouse_callback

float lastX = 960, lastY = 540; //Used in mouse_callback. Set to the middle of the screen

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

int currentTargetIndex = 0;
bool movingForward = true;

bool gamestart = false;

std::vector<glm::vec3> surfacePoints;

std::vector<Triangle> surfaceTriangles;




glm::vec3 lightPos(7.2f, 6.2f, 10.0f);

#pragma endregion

#pragma region Function Declarations
void setup(GLFWwindow*& window, unsigned& shaderProgram, unsigned& VBO, unsigned& VAO, unsigned& EBO,
           int& vertexColorLocation, int& value1);
void render(GLFWwindow* window, unsigned shaderProgram, unsigned VAO, int vertexColorLocation);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

void Gravity(const std::vector<Triangle>& surfaceTriangles);

void Parametric(glm::vec3 centerPoint);
void MapCurveToSurface();

void moveNPC();

glm::vec3 barycentricCoordinates(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 P);

float calculateHeightUsingBarycentric2(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, const glm::vec3& P);

glm::vec2 parametricCircle(float t, float radius) {
    float x = radius * cos(t);
    float y = radius * sin(t);
    return glm::vec2(x, y);
}




//Testing new functions for height
bool isPointAboveTriangleXZ(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, const glm::vec3& P);
Triangle findTriangleUnderneath(const std::vector<Triangle>& triangles, const glm::vec3& playerPosition);

#pragma endregion

#pragma region Settings

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

std::string vertexShaderSourceString = fileManager.readFile("../../../core/VertShader.vert");
std::string fragmentShaderSourceString = fileManager.readFile("../../../core/FragmentShader.frag");

const char *vertexShaderSource = vertexShaderSourceString.c_str();
const char *fragmentShaderSource = fragmentShaderSourceString.c_str();

std::string lightVertString = fileManager.readFile("../../../core/LightShader.vert");
std::string lightFragString = fileManager.readFile("../../../core/LightShader.frag");

const char *lightVert = lightVertString.c_str();
const char *lightFrag = lightFragString.c_str();

#pragma endregion


void CreateObjects()
{
    surface = Surface(20);
    surfaceTriangles = surface.GetTriangles();

    glm::vec3 centerPoint = glm::vec3(10.f, 15.f, 10.f);
    Parametric(centerPoint);

    surfacePoints.pop_back();
    EpicLine = Line(surfacePoints);

    PlayerCollision = Box(0.3f, Player);




    lightbox = Box(1.5f, Door);

    epicCube = Box(0.5f, Pickup);
    epicCube2 = Box(0.5f, Pickup);

    epicNPC = Box(0.5f, Pickup);

    epicNPC.SetPosition(centerPoint);

}

int main()
{





    GLFWwindow* window;
    unsigned shaderProgram, VBO, VAO, EBO;
    int vertexColorLocation, value1;
    
    
    setup(window, shaderProgram, VBO, VAO, EBO, vertexColorLocation, value1);
    
    CreateObjects();
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    render(window, shaderProgram, VAO, vertexColorLocation);

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


void setup(GLFWwindow*& window, unsigned& shaderProgram, unsigned& VBO, unsigned& VAO, unsigned& EBO,
           int& vertexColorLocation, int& value1)
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Compulsory 3", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        value1 = -1;
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        value1 = -1;
        return;
    }


    shader.CreateVertexShader(vertexShaderSource);
    shader.CreateFragmentShader(fragmentShaderSource);
    shader.LinkProgram();
    shaderProgram = shader.GetProgram();

    lightShader.CreateVertexShader(lightVert);
    lightShader.CreateFragmentShader(lightFrag);
    lightShader.LinkProgram();

    glUseProgram(shaderProgram);

    glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 1.0f, 0.5f, 0.31f);
    glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
    glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), MainCamera.cameraPos.x, MainCamera.cameraPos.y, MainCamera.cameraPos.z);
    
    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);  
    
    return;
}


void DrawObjects(unsigned shaderProgram)
{

    glUseProgram(lightShader.GetProgram());
    lightbox.Draw(lightShader.GetProgram());
    EpicLine.Draw(lightShader.GetProgram());

    glUseProgram(shaderProgram);
    surface.Draw(shaderProgram);

    PlayerCollision.Draw(shaderProgram);

    epicCube.Draw(shaderProgram);
    epicCube2.Draw(shaderProgram);

    epicNPC.Draw(shaderProgram);


    moveNPC();



}

void CameraView(std::vector<unsigned> shaderPrograms, glm::mat4 trans, glm::mat4 projection)
{
    for (unsigned shaderProgram : shaderPrograms)
    {
        glUseProgram(shaderProgram);

        int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        MainCamera.tick();

        glm::mat4 view;
        view = glm::lookAt(MainCamera.cameraPos, MainCamera.cameraPos + MainCamera.cameraFront, MainCamera.cameraUp);

        int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        // Pass the transformation matrix to the vertex shader
        unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
    }
}

void render(GLFWwindow* window, unsigned shaderProgram, unsigned VAO, int vertexColorLocation)
{
    glm::mat4 trans = glm::mat4(1.0f);
    glm::mat4 projection;

    lightbox.model = glm::mat4(1.0f);
    lightbox.model = glm::translate(lightbox.model, lightPos);
    lightbox.model = glm::scale(lightbox.model, glm::vec3(1.0001f));

    PlayerCollision.model = glm::scale(PlayerCollision.model, glm::vec3(0.01f));

    surface.model = glm::translate(surface.model, glm::vec3(0.0f, -1.2, 0.0f));
    surface.worldPosition = glm::vec3(0.0f, -1.2, 0.0f);

    epicCube.model = glm::translate(epicCube.model, glm::vec3(4.6f, -1.2f, 3.9f));
    epicCube2.model = glm::translate(epicCube.model, glm::vec3(9.4f, 1.1f, 11.f));


    std::cout << surfacePoints[0].x << " " << surfacePoints[0].y << " " << surfacePoints[0].z << std::endl;


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        //Field of view changes if inside house
        projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        std::vector<unsigned> shaderPrograms = {shaderProgram, lightShader.GetProgram()};
        CameraView(shaderPrograms, trans, projection);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniform4f(vertexColorLocation, 0.0f, 1.0f, 0.0f, 1.0f);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        glLineWidth(12);


        //collision following camera
        PlayerCollision.model = glm::mat4(1.0f); // Reset the model matrix


        glm::vec3 playerPos = MainCamera.cameraPos;
        playerPos.y -= 1.2f;
        PlayerCollision.model = glm::translate(PlayerCollision.model, playerPos);


        DrawObjects(shaderProgram);

        Gravity(surfaceTriangles);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();

    }
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{

    glm::vec3 cameraFrontXZ = glm::normalize(glm::vec3(MainCamera.cameraFront.x, 0.0f, MainCamera.cameraFront.z));

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5f * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            MainCamera.cameraPos += cameraSpeed * cameraFrontXZ;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            MainCamera.cameraPos -= cameraSpeed * cameraFrontXZ;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            MainCamera.cameraPos -= glm::normalize(glm::cross(MainCamera.cameraFront, MainCamera.cameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            MainCamera.cameraPos += glm::normalize(glm::cross(MainCamera.cameraFront, MainCamera.cameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            MainCamera.cameraPos += cameraSpeed * MainCamera.cameraUp; // Move camera up
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            MainCamera.cameraPos -= cameraSpeed * MainCamera.cameraUp; // Move camera down
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            ;
            MainCamera.cameraPos.y += 0.01;
        }


}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

//Alt av mouse input skjer her.
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
 {
    
     if (firstMouse)
     {
         lastX = xpos;
         lastY = ypos;
         firstMouse = false;
     }

     float xoffset = xpos - lastX;
     float yoffset = lastY - ypos; 
     lastX = xpos;
     lastY = ypos;
 
     float sensitivity = 0.1f;
     xoffset *= sensitivity;
     yoffset *= sensitivity;
 
     MainCamera.yaw   += xoffset;
     MainCamera.pitch += yoffset;
 
     if(MainCamera.pitch > 89.0f)
         MainCamera.pitch = 89.0f;
     if(MainCamera.pitch < -89.0f)
         MainCamera.pitch = -89.0f;
 
     glm::vec3 direction;
     direction.x = cos(glm::radians(MainCamera.yaw)) * cos(glm::radians(MainCamera.pitch));
     direction.y = sin(glm::radians(MainCamera.pitch));
     direction.z = sin(glm::radians(MainCamera.yaw)) * cos(glm::radians(MainCamera.pitch));
     MainCamera.cameraFront = glm::normalize(direction);
 }

void Gravity(const std::vector<Triangle>& surfaceTriangles)
{
    float gravity = 4.81f;

    glm::vec3 playerPosition = MainCamera.cameraPos;

    // Find the triangle directly beneath the player
    Triangle nearestTriangle = findTriangleUnderneath(surfaceTriangles, playerPosition);

}

glm::vec3 barycentricCoordinates(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 P) {


    glm::vec3 v0 = B - A, v1 = C - A, v2 = P - A;
    float d00 = glm::dot(v0, v0);
    float d01 = glm::dot(v0, v1);
    float d11 = glm::dot(v1, v1);
    float d20 = glm::dot(v2, v0);
    float d21 = glm::dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;

    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    return glm::vec3(u, v, w);
}


//EXPERIMENTAL CODE UNDERNEATH HERE!!!!!

Triangle findTriangleUnderneath(const std::vector<Triangle>& triangles, const glm::vec3& playerPosition) {
    Triangle nearestTriangle;
    float nearestDistance = (std::numeric_limits<float>::max)();

    for (const auto& triangle : triangles) {
        if (isPointAboveTriangleXZ(triangle.v0, triangle.v1, triangle.v2, playerPosition)) {

            float heightOnTriangle = calculateHeightUsingBarycentric2(triangle.v0, triangle.v1, triangle.v2, playerPosition);
            float distance = playerPosition.y - heightOnTriangle;

            if (distance >= 0.0f && distance < nearestDistance) {
                nearestDistance = distance;
                nearestTriangle = triangle;
            }
        }
    }

    return nearestTriangle;
}

bool isPointAboveTriangleXZ(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, const glm::vec3& P) {

    glm::vec3 P_proj(P.x, 0.0f, P.z);


    glm::vec3 A_proj(A.x, 0.0f, A.z);
    glm::vec3 B_proj(B.x, 0.0f, B.z);
    glm::vec3 C_proj(C.x, 0.0f, C.z);


    glm::vec3 baryCoords = barycentricCoordinates(A_proj, B_proj, C_proj, P_proj);



    bool isWithinTriangle = baryCoords.x >= 0.0f && baryCoords.y >= 0.0f && baryCoords.z >= 0.0f;

    //std::cout << "BaryCoords: " << baryCoords.x << " " << baryCoords.y << " " << baryCoords.z << std::endl;
    bool test = baryCoords.x >= 0.0f && baryCoords.y >= 0.0f && baryCoords.z >= 0.0f;

    if (test && gamestart)
    {
        float heightOnSurface = calculateHeightUsingBarycentric2(A, B, C, P);

        // std::cout << MainCamera.cameraPos.x << " " << MainCamera.cameraPos.y << " " << MainCamera.cameraPos.z << std::endl;
        MainCamera.cameraPos.y = heightOnSurface + surface.worldPosition.y *-1;
    }
    return baryCoords.x >= 0.0f && baryCoords.y >= 0.0f && baryCoords.z >= 0.0f;
}

void Parametric(glm::vec3 centerPoint) {

    std::vector<glm::vec2> curvePoints;
    float radius = 3.0f;
    int samples = 50;
    for (int i = 0; i <= samples-1; ++i) {
        float t = 2.0f * glm::pi<float>() * i / static_cast<float>(samples);
        glm::vec2 point = parametricCircle(t, radius);


        glm::vec3 point3D(point.x, 0.0f, point.y);


        point3D += centerPoint;

        surfacePoints.push_back(point3D);

        MapCurveToSurface();

    }
}

void MapCurveToSurface() {
    for (glm::vec3& point : surfacePoints) {;
        float nearestDistance = (std::numeric_limits<float>::max)();
        float adjustedY = 0.0f;

        for (const Triangle& triangle : surfaceTriangles) {
            if (isPointAboveTriangleXZ(triangle.v0, triangle.v1, triangle.v2, point)) {

                //cout triangle v0 v1 v2
                // std::cout << "Triangle v0: " << triangle.v0.x << " " << triangle.v0.y << " " << triangle.v0.z << std::endl;

                float heightOnSurface = calculateHeightUsingBarycentric2(triangle.v0, triangle.v1, triangle.v2, point);


                float distanceToSurface = std::abs(heightOnSurface - point.y);

                if (distanceToSurface < nearestDistance) {
                    nearestDistance = distanceToSurface;
                    adjustedY = heightOnSurface;
                }
                break;
            }
        }


        point.y = adjustedY;
    }
    gamestart = true;
}


void moveNPC() {
    // Get the current target point
    glm::vec3 readposition = epicNPC.GetWorldPosition();


    glm::vec3 targetPoint = surfacePoints[currentTargetIndex+1];


    // Calculate the direction vector from the NPC to the target point
    glm::vec3 direction = glm::normalize(targetPoint - readposition);

    // Move NPC towards target
    float speed = 2.0f * deltaTime; // Adjust the speed as necessary
    glm::vec3 tempposition = epicNPC.GetWorldPosition();
    tempposition += direction * speed;
    epicNPC.SetPosition(tempposition);

    // Check if the NPC is close enough to the target point
    float distanceToTarget = glm::distance(epicNPC.GetWorldPosition(), targetPoint);
    if (distanceToTarget < 0.1f) { // Threshold value
        // Move to the next point
        currentTargetIndex++;

        // Loop back to start
        if (currentTargetIndex >= surfacePoints.size()-1) {
            currentTargetIndex = 0;
        }
    }
}


float calculateHeightUsingBarycentric2(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, const glm::vec3& P) {
    // Calculate barycentric coordinates for P within the triangle ABC.
    glm::vec3 baryCoords = barycentricCoordinates(A, B, C, P);

    // Interpolate Y cood at P.
    float height = A.y * baryCoords.x + B.y * baryCoords.y + C.y * baryCoords.z;
    return height;
}


