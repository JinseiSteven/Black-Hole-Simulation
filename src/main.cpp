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