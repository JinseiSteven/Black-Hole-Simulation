//
// Created by StephanVisser on 10/26/2025.
//

#ifndef BLACK_HOLE_SIMULATION_INPUTHANDLER_H
#define BLACK_HOLE_SIMULATION_INPUTHANDLER_H

class Camera;
class GLFWwindow;


class InputHandler {
    GLFWwindow* m_window;
    Camera* m_camera;
    bool m_input_enabled;
    bool m_ctrl_held;
    bool m_first_movement;

    double m_last_mouse_x{0.0f};
    double m_last_mouse_y{0.0f};
public:
    InputHandler(GLFWwindow* window, Camera* camera);
    void toggle_input(bool activate);
    void CursorPosCallback(double xpos, double ypos);
    void MouseButtonCallback(int button, int action, int mods);
    void KeyCallback(int key, int scancode, int action, int mods);
    static void CursorPosCallbackWrapper(GLFWwindow* window, double xpos, double ypos);
    static void MouseButtonCallbackWrapper(GLFWwindow* window, int button, int action, int mods);
    static void KeyCallbackWrapper(GLFWwindow* window, int key, int scancode, int action, int mods);
};


#endif //BLACK_HOLE_SIMULATION_INPUTHANDLER_H