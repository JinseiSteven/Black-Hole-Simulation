//
// Created by StephanVisser on 1/19/2026.
//

#include <string>
#include <stdexcept>
#include <GL/glew.h>
#include <cassert>

#include "Window.h"
#include "InputState.h"
#include "Config.h"
#include "stb_image.h"


Window::Window(const int width, const int height, const std::string& title, InputState& inputState) :
    m_window(nullptr),
    m_width(width),
    m_height(height),
    m_inputState(inputState){
    initGLFW(title);
    initGL();
    initInputCallbacks();
}

Window::~Window() {
    if (m_window != nullptr) {
        glfwDestroyWindow(m_window);
    }
    glfwTerminate();
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_window);
}

void Window::swapBuffers() const {
    glfwSwapBuffers(m_window);
}

void Window::SetShouldClose(const bool value) const {
    glfwSetWindowShouldClose(m_window, value ? GLFW_TRUE : GLFW_FALSE);
}

void Window::SetIcon(const char* path) const {
    int w, h, channels;
    unsigned char* pixels = stbi_load(path, &w, &h, &channels, 4);
    if (pixels) {
        GLFWimage icon{w, h, pixels};
        glfwSetWindowIcon(m_window, 1, &icon);
        stbi_image_free(pixels);
    }
}

void Window::SetCursorMode(const bool enabled) const {
    if (enabled) {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    else {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

void Window::initGLFW(const std::string& title) {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    // setting up the context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_window = glfwCreateWindow(
        m_width,
        m_height,
        title.c_str(),
        nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window\n");
    }
    glfwMakeContextCurrent(m_window);
    ResetCursorTracking();
}


void Window::initGL() {
    // ensure that the window is actually created first (should put asserts more often lol)
    assert(m_window != nullptr && "initGL called before initGLFW");

    // this so we can use modern GL functions, not just old ones
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        glfwDestroyWindow(m_window);
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLEW\n");
    }

    // now we also store a pointer to the current raytrace engine inside the window, so we can fetch it and its
    // components from any GLFW callback
    glfwSetWindowUserPointer(m_window, this);

    // actually making the viewport now
    glViewport(0, 0, m_width, m_height);
}

void Window::initInputCallbacks() const {
    glfwSetMouseButtonCallback(m_window, MouseButtonCallbackWrapper);
    glfwSetCursorPosCallback(m_window, CursorPosCallbackWrapper);
    glfwSetKeyCallback(m_window, KeyCallbackWrapper);
}

void Window::CursorPosCallback(double xpos, double ypos) {
    m_inputState.mouse_delta_x += xpos - m_last_x;
    m_inputState.mouse_delta_y += ypos - m_last_y;
    m_last_x = xpos;
    m_last_y = ypos;
}

void Window::MouseButtonCallback(int button, int action, int mods) {

    int button_flag = 0;
    if (button == GLFW_MOUSE_BUTTON_LEFT) button_flag = LEFT_MOUSE_FLAG;
    else if (button == GLFW_MOUSE_BUTTON_RIGHT) button_flag = RIGHT_MOUSE_FLAG;
    if (button_flag == 0) return;

    if (action == GLFW_PRESS) {
        m_inputState.keys_pressed_mask |= button_flag;
        m_inputState.keys_held_mask |= button_flag;
    }
    else if (action == GLFW_RELEASE) {
        m_inputState.keys_held_mask &= ~button_flag;
    }
}

void Window::ScrollCallback(double xoffset, double yoffset) {
    // TODO actually implement this if we need it
}

void Window::KeyCallback(int key, int scancode, int action, int mods) {
    int key_flag = 0;
    if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL) key_flag = CTRL_FLAG;
    else if (key == GLFW_KEY_ESCAPE) key_flag = ESC_FLAG;
    else if (key == GLFW_KEY_TAB) key_flag = TAB_FLAG;
    if (key_flag == 0) return;

    if (action == GLFW_PRESS) {
        m_inputState.keys_pressed_mask |= key_flag;
        m_inputState.keys_held_mask |= key_flag;
    }
    else if (action == GLFW_RELEASE) {
        m_inputState.keys_held_mask &= ~key_flag;
    }
}

// function callback wrappers since the GLFW callbacks require static functions
void Window::CursorPosCallbackWrapper(GLFWwindow* window, double xpos, double ypos) {
    if (Window* window_o = static_cast<Window*>(glfwGetWindowUserPointer(window))) {
        window_o->CursorPosCallback(xpos, ypos);
    }
}

void Window::MouseButtonCallbackWrapper(GLFWwindow* window, int button, int action, int mods) {
    if (Window* window_o = static_cast<Window*>(glfwGetWindowUserPointer(window))) {
        window_o->MouseButtonCallback(button, action, mods);
    }
}

void Window::ScrollCallbackWrapper(GLFWwindow* window, double xoffset, double yoffset) {
    if (Window* window_o = static_cast<Window*>(glfwGetWindowUserPointer(window))) {
        window_o->ScrollCallback(xoffset, yoffset);
    }
}


void Window::KeyCallbackWrapper(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (Window* window_o = static_cast<Window*>(glfwGetWindowUserPointer(window))) {
        window_o->KeyCallback(key, scancode, action, mods);
    }
}


