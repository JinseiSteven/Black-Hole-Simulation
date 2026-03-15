// need to tell the OS to use the dedicated GPU instead of the integrated one if present
extern "C" {
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) unsigned long AmdPowerXpressRequestHighPerformance = 0x00000001;
}

#include <iostream>
#include <filesystem>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Application.h"

int main(int argc, char* argv[]) {
    std::filesystem::current_path(
        std::filesystem::path(argv[0]).parent_path()
    );

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