//
// Created by StephanVisser on 10/26/2025.
//

#include "InputHandler.h"
#include "InputState.h"
#include "Window.h"
#include "CameraController.h"
#include "UISystem.h"


InputHandler::InputHandler(Window& window, CameraController& camera_controller, UISystem& ui_system) :
    m_camera_controller(camera_controller),
    m_window{window},
    m_ui_system{ui_system} { }

void InputHandler::ProcessInput(InputState &input_state) {

    // escape will stop the application
    if (input_state.keys_pressed_mask & ESC_FLAG) {
        m_window.SetShouldClose(true);
        return;
    }

    // tab will toggle the settings window
    if (input_state.keys_pressed_mask & TAB_FLAG) {
        m_ui_system.ToggleSettingsWindow();
    }

    bool should_enable = input_state.keys_held_mask & LEFT_MOUSE_FLAG;
    if (should_enable != m_input_enabled) toggle_input(should_enable);
    if (m_input_enabled) {
        // we want to ignore the first frame of movement when it has been re-enabled, to prevent big jumps
        if (m_first_movement) {
            input_state.mouse_delta_x = 0;
            input_state.mouse_delta_y = 0;
            m_first_movement = false;
        }
        m_camera_controller.ProcessInput(input_state);
    }
}

void InputHandler::Update(float delta_time) {
    m_camera_controller.Update(delta_time);
    // ... any other components we need to update
}

void InputHandler::toggle_input(const bool activate) {
    if (activate) {
        m_window.SetCursorMode(false);
        m_input_enabled = true;
        m_first_movement = true;
    }
    else {
        m_window.SetCursorMode(true);
        m_input_enabled = false;
    }
    m_window.ResetCursorTracking();
}
