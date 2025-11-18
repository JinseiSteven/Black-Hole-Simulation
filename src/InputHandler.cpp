//
// Created by StephanVisser on 10/26/2025.
//

#include "InputHandler.h"

#include <GLFW/glfw3.h>

#include "RayTraceEngine.h"
#include "Camera.h"

#include <iostream>

InputHandler::InputHandler(GLFWwindow* window, Camera* camera) :
    m_window(window),
    m_camera(camera),
    m_input_enabled(false),
    m_ctrl_held(false),
    m_first_movement(true) { }

void InputHandler::toggle_input(const bool activate) {
    if (activate) {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        m_input_enabled = true;
        m_first_movement = true;
    }
    else {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        m_input_enabled = false;
    }
}

void InputHandler::CursorPosCallback(const double xpos, const double ypos) {
    if (!m_input_enabled) {
        return;
    }

    if (m_first_movement) {
        m_last_mouse_x = xpos;
        m_last_mouse_y = ypos;
        m_first_movement = false;
    }

    // y is flipped since y coords go bottom-up
    double xoffset = xpos - m_last_mouse_x;
    double yoffset = m_last_mouse_y - ypos;

    m_last_mouse_x = xpos;
    m_last_mouse_y = ypos;

    if (m_camera) {
        if (m_ctrl_held) {
            m_camera->ProcessZoom(static_cast<float>(yoffset));
        }
        else {
            m_camera->ProcessMouseMovement(static_cast<float>(xoffset), static_cast<float>(yoffset));
        }
    }
}

void InputHandler::MouseButtonCallback(int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        toggle_input(action == GLFW_PRESS);
    }
}

void InputHandler::KeyCallback(int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL) {
        m_ctrl_held = (action == GLFW_PRESS || action == GLFW_REPEAT);
    }
}

void InputHandler::CursorPosCallbackWrapper(GLFWwindow* window, double xpos, double ypos) {
    if (const RayTraceEngine* engine = static_cast<RayTraceEngine*>(glfwGetWindowUserPointer(window))) {
        engine->GetInputHandler()->CursorPosCallback(xpos, ypos);
    }
}

void InputHandler::MouseButtonCallbackWrapper(GLFWwindow* window, int button, int action, int mods) {
    if (const RayTraceEngine* engine = static_cast<RayTraceEngine*>(glfwGetWindowUserPointer(window))) {
        engine->GetInputHandler()->MouseButtonCallback(button, action, mods);
    }
}

void InputHandler::KeyCallbackWrapper(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (const RayTraceEngine* engine = static_cast<RayTraceEngine*>(glfwGetWindowUserPointer(window))) {
        engine->GetInputHandler()->KeyCallback(key, scancode, action, mods);
    }
}
