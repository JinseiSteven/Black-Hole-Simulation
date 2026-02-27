//
// Created by StephanVisser on 1/21/2026.
//

#include "Application.h"
#include "Config.h"
#include "Utils.h"


Application::Application() : Application(BASE_SCREEN_WIDTH, BASE_SCREEN_HEIGHT) { }

Application::Application(const int screen_width, const int screen_height) :
    m_camera(
        Config::FOV_Y,
        static_cast<float>(screen_width) / static_cast<float>(screen_height),
        Config::Z_NEAR,
        Config::Z_FAR),
    m_window(Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, "Schwarzschild Black Hole", m_input_state),
    m_camera_controller(m_camera, m_settings),
    m_ui_system(m_window, m_settings),
    m_input_handler(m_window, m_camera_controller, m_ui_system),
    m_renderer(
        m_camera,
        m_settings,
        static_cast<unsigned int>(static_cast<float>(screen_width) * m_settings.GetRenderScale()),
        static_cast<unsigned int>(static_cast<float>(screen_height) * m_settings.GetRenderScale()),
        Config::SCREENQUAD_VERT.c_str(),
        Config::SCREENQUAD_FRAG.c_str(),
        Config::RADIALMESH_VERT.c_str(),
        Config::RADIALMESH_FRAG.c_str()),
    m_simulation(
        m_settings,
        static_cast<unsigned int>(static_cast<float>(screen_width) * m_settings.GetRenderScale()),
        static_cast<unsigned int>(static_cast<float>(screen_height) * m_settings.GetRenderScale()),
        m_renderer.GetScreenTextureID(),
        Config::COMPUTE_RAYTRACE.c_str(),
        Config::COMPUTE_PINN.c_str())
{
    if (!CheckRenderScale(Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT, Config::RENDER_SCALE)) {
        throw std::runtime_error("Render scale not supported. The render texture needs to be divisible by the GPU workgroups");
    }
    glfwSetWindowTitle(m_window.getWindow(), "Schwarzschild Black Hole Simulation");
    m_simulation.UpdatePlanetsData(Utils::LoadPlanetsFromFile(Config::PLANETS_TXT_FILE));
    RebuildRadialMesh();
}

int Application::Run() {
    while (!m_window.shouldClose()) {
        Step();
    }
    return 0;
}

void Application::Step() {
    const float delta_time = m_clock.tick();

    glfwPollEvents();

    m_ui_system.NewFrame();

    if (m_ui_system.IsPinnView()) {
        PINNRenderCycle();
    }
    else {
        BaseRenderCycle(delta_time);
    }

    // ui windows
    m_ui_system.CreateWindows();

    // rendering
    m_renderer.draw();
    m_ui_system.Draw();

    m_window.swapBuffers();
    m_input_state.clear();
}

void Application::BaseRenderCycle(const float delta_time) {
    if (!m_ui_system.WantCaptureMouse() && !m_input_state.consumed) {
        // inputhandler will consume the input then if its not consumed already
        m_input_handler.ProcessInput(m_input_state);
    }
    m_input_handler.Update(delta_time);

    if (m_settings.GetRenderVersion() != m_last_render_version) {
        RebuildRadialMesh();
        m_last_render_version = m_settings.GetRenderVersion();
    }

    // simulating
    m_simulation.step(m_camera);
}

void Application::PINNRenderCycle() {
    if (!m_pinn_active) {
        m_pinn_active = true;
        m_pinn_row_cursor = 0;

        // making it black so we get a nice clear render setup
        m_simulation.ClearOutputTexture();
    }

    unsigned int row_count = std::min(m_simulation.GetRenderHeight() - m_pinn_row_cursor, Config::SCAN_ROW_BATCH_SIZE);
    m_simulation.RenderPinnRows(m_camera, m_pinn_row_cursor, row_count);
}

void Application::RebuildRadialMesh() {
    const int num_rings = m_settings.GetRadialMeshRings();
    const float inner_radius = m_settings.GetRadialMeshInnerRadius();
    const float outer_radius = m_settings.GetRadialMeshOuterRadius();

    m_simulation.UpdateRadialHeightMap(
        num_rings,
        inner_radius,
        outer_radius);
    m_renderer.RebuildRadialMesh(
        num_rings,
        m_settings.GetRadialMeshSpokes(),
        inner_radius,
        outer_radius);

    m_renderer.UpdateRadialHeightmap(m_simulation.GetRadialHeightMap());
}

// we simply use this function to verify during construction whether or not the chosen rendering width and height
// are supported by the GPU workgroup distribution
bool Application::CheckRenderScale(const int screen_width, const int screen_height, const float render_scale) const{
    // need to round the pixels up
    int trace_width = static_cast<int>(std::round(static_cast<float>(screen_width) * render_scale));
    int trace_height = static_cast<int>(std::round(static_cast<float>(screen_height) * render_scale));

    // we check whether we can divide the pixels up nicely among the workgroups
    bool width_is_divisible = (trace_width % Config::WORKGROUP_SIZE_X) == 0;
    bool height_is_divisible = (trace_height % Config::WORKGROUP_SIZE_Y) == 0;

    return width_is_divisible && height_is_divisible;
}

