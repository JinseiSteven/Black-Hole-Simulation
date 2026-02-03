//
// Created by StephanVisser on 10/26/2025.
//

#ifndef BLACK_HOLE_SIMULATION_INPUTHANDLER_H
#define BLACK_HOLE_SIMULATION_INPUTHANDLER_H

class Camera;
class CameraController;
class GLFWwindow;
class Window;
class UISystem;
struct InputState;

class InputHandler {
public:
    InputHandler(Window& window, CameraController& camera_controller, UISystem& ui_system);
    void ProcessInput(InputState& input_state);
    void Update(float delta_time);
    void toggle_input(bool activate);
private:
    CameraController& m_camera_controller;
    Window& m_window;
    UISystem& m_ui_system;
    bool m_input_enabled{false};
    bool m_ctrl_held{false};
    bool m_first_movement{true};

};


#endif //BLACK_HOLE_SIMULATION_INPUTHANDLER_H