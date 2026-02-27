//
// Created by StephanVisser on 1/21/2026.
//

#ifndef BLACK_HOLE_SIMULATION_APPLICATION_H
#define BLACK_HOLE_SIMULATION_APPLICATION_H

#include "Window.h"
#include "Clock.h"
#include "InputState.h"
#include "InputHandler.h"
#include "CameraController.h"
#include "Settings.h"
#include "Camera.h"
#include "Simulation.h"
#include "Renderer.h"
#include "UISystem.h"


class Application {
public:
    Application();
    Application(int screen_width, int screen_height);
    int Run();
    void Step();
private:
    InputState m_input_state;
    Settings m_settings;
    Camera m_camera;
    Window m_window;
    Clock m_clock;
    CameraController m_camera_controller;
    UISystem m_ui_system;
    InputHandler m_input_handler;
    Renderer m_renderer;
    Simulation m_simulation;

    unsigned int m_last_render_version{0xFFFFFFFF};

    bool m_pinn_active{false};
    unsigned int m_pinn_row_cursor{0};

    void PINNRenderCycle();
    void BaseRenderCycle(float delta_time);

    void RebuildRadialMesh();
    [[nodiscard]] bool CheckRenderScale(int screen_width, int screen_height, float render_scale) const;

    constexpr static int BASE_SCREEN_WIDTH = 800;
    constexpr static int BASE_SCREEN_HEIGHT = 600;
};


#endif //BLACK_HOLE_SIMULATION_APPLICATION_H