#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

extern GLFWwindow* window;

bool lightOn = true;

// Camera spherical coordinates
float radius = -50.0f;       // Distance from the origin
float theta = 3.14f; // Horizontal angle
float phi = 3.14f / 6.0f;   // Vertical angle

float cameraSpeed = 1.0f;   // Camera movement speed

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix() {
    return ViewMatrix;
}

glm::mat4 getProjectionMatrix() {
    return ProjectionMatrix;
}

void computeMatricesFromInputs() {
    // Poll keyboard inputs for radial movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        radius += cameraSpeed; // Move closer to the origin
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        radius -= cameraSpeed; // Move farther from the origin
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        theta -= cameraSpeed * 0.025f; // Rotate left
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        theta += cameraSpeed * 0.025f; // Rotate right
    }

    // Rotate the camera vertically with up and down arrow keys
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        phi += cameraSpeed * 0.025f; // Rotate up (increase phi)
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        phi -= cameraSpeed * 0.025f; // Rotate down (decrease phi)
    }

    // Orbit around the chessboard using left and right arrow keys
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        theta -= cameraSpeed * 0.025f; // Orbit left around the chessboard
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        theta += cameraSpeed * 0.025f; // Orbit right around the chessboard
    }

    // Clamp the vertical angle to prevent flipping (phi should be between -90° and +90°)
    if (phi > 3.14f / 2.0f) phi = 3.14f / 2.0f;
    if (phi < -3.14f / 2.0f) phi = -3.14f / 2.0f;

    // Toggle specular and diffuse lighting with 'L'
    static bool lKeyPressed = false;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        if (!lKeyPressed) {
            lightOn = !lightOn;
            std::cout << (lightOn ? "Light ON" : "Light OFF") << std::endl;
        }
        lKeyPressed = true;
    }
    else {
        lKeyPressed = false;
    }

    // Compute the camera position in Cartesian coordinates
    float x = radius * cos(phi) * sin(theta);
    float y = radius * sin(phi);
    float z = radius * cos(phi) * cos(theta);

    // Set the camera to look at the origin (center of the chessboard)
    glm::vec3 cameraPos = glm::vec3(x, y, z);
    glm::vec3 target = glm::vec3(0, 0, 0);
    glm::vec3 up = glm::vec3(0, 1, 0); // Up vector along Y-axis

    // Update the view matrix to reflect the camera's new position and orientation
    ViewMatrix = glm::lookAt(cameraPos, target, up);

    // Projection matrix: Field of View, Aspect Ratio, Near and Far clipping planes
    float FoV = 60.0f;
    ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);

    // Close the window if the ESC key is pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
}

