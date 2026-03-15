//
// Created by StephanVisser on 1/19/2026.
//

#ifndef BLACK_HOLE_SIMULATION_WINDOW_H
#define BLACK_HOLE_SIMULATION_WINDOW_H

#include <memory>
#include <GLFW/glfw3.h>
#include <string>

struct InputState;


class Window {
public:
    Window(int width, int height, const std::string& title, InputState& inputState);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    [[nodiscard]] bool shouldClose() const;
    void swapBuffers() const;
    void SetShouldClose(bool value) const;
    void SetCursorMode(bool enabled) const;
    void SetIcon(const char* path) const;

    [[nodiscard]] GLFWwindow* getWindow() const { return m_window; }
    [[nodiscard]] int getWidth() const { return m_width; }
    [[nodiscard]] int getHeight() const { return m_height; }

    void CursorPosCallback(double xpos, double ypos);
    void MouseButtonCallback(int button, int action, int mods);
    void ScrollCallback(double xoffset, double yoffset);
    void KeyCallback(int key, int scancode, int action, int mods);

    static void CursorPosCallbackWrapper(GLFWwindow* window, double xpos, double ypos);
    static void MouseButtonCallbackWrapper(GLFWwindow* window, int button, int action, int mods);
    static void ScrollCallbackWrapper(GLFWwindow* window, double xoffset, double yoffset);
    static void KeyCallbackWrapper(GLFWwindow* window, int key, int scancode, int action, int mods);

    void ResetCursorTracking() {
        glfwGetCursorPos(m_window, &m_last_x, &m_last_y);
    }

private:
    GLFWwindow* m_window;
    int m_width;
    int m_height;
    InputState& m_inputState;
    double m_last_x{0};
    double m_last_y{0};

    void initGLFW(const std::string& title);
    void initGL();
    void initInputCallbacks() const;
};


#endif //BLACK_HOLE_SIMULATION_WINDOW_H