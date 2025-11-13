cmake_minimum_required(VERSION 3.28.3)
project(black_hole_simulation)

set(CMAKE_CXX_STANDARD 20)

find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# setting up the dependencies
set(DEPS
        glfw
        GLEW::GLEW
        glm::glm
        OpenGL::GL
)

# also setting up the source files
set(SOURCES
        src/main.cpp
        src/Camera.cpp
        src/ComputeShader.cpp
        src/InputHandler.cpp
        src/RayTraceEngine.cpp
        src/Renderer.cpp
        src/Settings.cpp
        src/Shader.cpp
        src/Simulation.cpp
)

add_executable(black_hole_simulation ${SOURCES})
target_link_libraries(black_hole_simulation PRIVATE ${DEPS})

# adding our include folder
target_include_directories(black_hole_simulation PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
