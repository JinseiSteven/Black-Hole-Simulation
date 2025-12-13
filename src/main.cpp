#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Config.h"
#include "RayTraceEngine.h"

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    // we will do camera movement processing here as well
}

int main() {

    try {
        RayTraceEngine engine(Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT);
        while (!glfwWindowShouldClose(engine.GetWindow())) {
            engine.step();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}