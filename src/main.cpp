#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Application.h"

int main() {
    try {
        Application app;
        app.Run();
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

// TODO REMOVE?
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}