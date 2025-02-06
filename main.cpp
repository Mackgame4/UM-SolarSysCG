#include "config.h"
#include <GLFW/glfw3.h>
#include <iostream>

// Window size
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Function prototypes
void renderScene();
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

// Global window pointer
GLFWwindow* window;

int main(int argc, char** argv) {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // Set GLFW window hints (optional, for OpenGL version)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    // Make the OpenGL context current
    glfwMakeContextCurrent(window);
    
    // Set the viewport and callback for resizing
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Render scene
        renderScene();

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(1);
}

// Callback for window resizing
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Dummy render function (Replace with actual rendering)
void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
