
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <windows.h>

#include "LightBox.h"
#include "Surface.h"
#include "core/Camera.h"
#include "core/FileManager.h"
#include "Mesh/Plane.h"
#include "core/Shader.h"
#include "Mesh/Box.h"
#include "Mesh/NPC.h"

#pragma region Public Variables

Camera MainCamera;
FileManager fileManager;
Shader shader;
Shader lightShader;
Plane plane;

Box PlayerCollision;

Box lightbox;

Surface surface;


bool firstMouse = true; // Used in mouse_callback

float lastX = 960, lastY = 540; //Used in mouse_callback. Set to the middle of the screen

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
int pickupcount = 0;

float playervelX = 0;
float playervelY = 0;

bool isJumping = false;




glm::vec3 lightPos(7.2f, 6.2f, 10.0f);

#pragma endregion

#pragma region Function Declarations
void setup(GLFWwindow*& window, unsigned& shaderProgram, unsigned& VBO, unsigned& VAO, unsigned& EBO,
               int& vertexColorLocation, int& value1, std::vector<float> floats);
void render(GLFWwindow* window, unsigned shaderProgram, unsigned VAO, int vertexColorLocation, std::vector<Vertex> points);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// void Gravity();
void Gravity(const std::vector<Triangle>& surfaceTriangles);

glm::vec3 barycentricCoordinates(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 P);

float calculateHeightUsingBarycentric(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 P);

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
    plane.CreateMeshPlane();
    PlayerCollision = Box(0.7f, Player);

    surface = Surface(20);
    
    //Wall1 = Box(-1.5, -0.7, -0.05, 1.5, 1, 0.05, House);
    
    //PlayerCollision = Box(-0.1f, -0.1f, -0.1f, 0.1f, 0.2f, 0.1f, Player);


    lightbox = Box(1.5f, Door);
}

int main()
{



    std::vector<Vertex> points = fileManager.readPointsFromFile("../../../core/NPCPoints.txt");
    std::vector<float> floats = fileManager.convertPointsToFloats(points, 1/9.9f);

    GLFWwindow* window;
    unsigned shaderProgram, VBO, VAO, EBO;
    int vertexColorLocation, value1;
    
    
    setup(window, shaderProgram, VBO, VAO, EBO, vertexColorLocation, value1, floats);
    
    CreateObjects();
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    render(window, shaderProgram, VAO, vertexColorLocation, points);

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
               int& vertexColorLocation, int& value1, std::vector<float> floats)
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

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Oppgave1", NULL, NULL);
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


void DrawObjects(unsigned shaderProgram, std::vector<Vertex> points)
{
    glDrawArrays(GL_LINE_STRIP, 0, points.size());
    //plane.DrawPlane(shaderProgram);

    glUseProgram(lightShader.GetProgram());
    lightbox.Draw(lightShader.GetProgram());

    glUseProgram(shaderProgram);
    surface.Draw(shaderProgram);

        
    //box.Draw(shaderProgram, 0, -1.f, 0);

}

void CollisionCheck()
{
    /*
    if (PlayerCollision.CheckCollision(&door))
    {
        std::cout << "Collision with door" << std::endl;
        MainCamera.cameraPos = glm::vec3(3.88911f, -5.91243f, 3.82015f);
        isInsideHouse = true;
    }
     */
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

//Alt av rendering skjer her. Inkoudert da den while loopen
void render(GLFWwindow* window, unsigned shaderProgram, unsigned VAO, int vertexColorLocation, std::vector<Vertex> points)
{
    glm::mat4 trans = glm::mat4(1.0f);
    glm::mat4 projection;

    plane.model = glm::translate(plane.model, glm::vec3(0.0f, -1.2f, 0.0f));

    lightbox.model = glm::mat4(1.0f);
    lightbox.model = glm::translate(lightbox.model, lightPos);
    lightbox.model = glm::scale(lightbox.model, glm::vec3(1.0001f));

    surface.model = glm::translate(surface.model, glm::vec3(0.0f, -1.2, 0.0f));
    surface.worldPosition = glm::vec3(0.0f, -1.2, 0.0f);


    // surface.model = glm::scale(surface.model, glm::vec3(7.11f));


    std::vector<Triangle> surfaceTriangles = surface.GetTriangles();
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
        PlayerCollision.model = glm::translate(PlayerCollision.model, MainCamera.cameraPos);


        DrawObjects(shaderProgram, points);

        CollisionCheck();

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
            playervelY = 2;;
            MainCamera.cameraPos.y += 0.01;
            isJumping = true;
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

/*void Gravity()
{
    float gravity = 4.81f;

    float velocity = 0;

    if (MainCamera.cameraPos.y < -0.2f)
    {
        MainCamera.cameraPos.y = -0.20007f;
        if (isJumping)
        {
            isJumping = false;
            velocity = 0;
            playervelY = 0;
        }
    }
    else
    {
        velocity = playervelY - gravity * deltaTime;


        if (velocity < -3.0f) velocity = -3.0f;

        std::cout << "Velocity: " << playervelY << std::endl;

    }
    playervelY = velocity;
    MainCamera.cameraPos.y += playervelY * deltaTime;

    // std::cout << "Camera X: " << MainCamera.cameraPos.x << " Camera Y: " << MainCamera.cameraPos.y << " Camera Z: " << MainCamera.cameraPos.z << std::endl;


}*/

void Gravity(const std::vector<Triangle>& surfaceTriangles)
{
    float gravity = 4.81f;

    // Get the current player position
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

float calculateHeightUsingBarycentric(glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 P) {
    A += surface.worldPosition;
    B += surface.worldPosition;
    C += surface.worldPosition;


    glm::vec3 baryCoords = barycentricCoordinates(A, B, C, P);
    glm::vec3 pos = surface.worldPosition;


    return (A.y * baryCoords.x + B.y * baryCoords.y + C.y * baryCoords.z);
}

//EXPERIMENTAL CODE UNDERNEATH HERE!!!!!

Triangle findTriangleUnderneath(const std::vector<Triangle>& triangles, const glm::vec3& playerPosition) {
    Triangle nearestTriangle;
    float nearestDistance = (std::numeric_limits<float>::max)();

    for (const auto& triangle : triangles) {
        if (isPointAboveTriangleXZ(triangle.v0, triangle.v1, triangle.v2, playerPosition)) {

            float heightOnTriangle = calculateHeightUsingBarycentric(triangle.v0, triangle.v1, triangle.v2, playerPosition);
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

    if (test)
    {
        float heightOnSurface = calculateHeightUsingBarycentric(A, B, C, P);
        std::cout << "Height on surface: " << heightOnSurface << std::endl;

        MainCamera.cameraPos.y = heightOnSurface + surface.worldPosition.y *-1;
    }
    return baryCoords.x >= 0.0f && baryCoords.y >= 0.0f && baryCoords.z >= 0.0f;
}



