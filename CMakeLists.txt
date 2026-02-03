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

set(IMGUI_DIR "external/imgui")

set(IMGUI_SRC
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# also setting up the source files
set(SOURCES
        src/main.cpp
        src/Application.cpp
        src/Camera.cpp
        src/CameraController.cpp
        src/Clock.cpp
        src/ComputeShader.cpp
        src/InputHandler.cpp
        src/Renderer.cpp
        src/Settings.cpp
        src/Shader.cpp
        src/Simulation.cpp
        src/UISystem.cpp
        src/Utils.cpp
        src/Window.cpp
        external/stb_image/stb_impl.cpp
        ${IMGUI_SRC}
)

add_executable(black_hole_simulation ${SOURCES})
target_link_libraries(black_hole_simulation PRIVATE ${DEPS})

# adding our include folder
target_include_directories(black_hole_simulation PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external/stb_image
        ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends
)
