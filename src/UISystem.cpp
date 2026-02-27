//
// Created by StephanVisser on 29-Jan-26.
//

#include <cfloat>
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "UISystem.h"
#include "Window.h"
#include "Settings.h"


UISystem::UISystem(const Window& window, Settings& settings) :
    m_settings(settings)
{
    IMGUI_CHECKVERSION();
    // allocates the internal state structure inside of memory
    ImGui::CreateContext();

    // (have to do this after creating the context, so cant be in the thingy above)
    // getting a reference to the input/output system, which reads it from GLFW kinda like us
    m_io = &ImGui::GetIO();
    // we do this to disable imgui.ini persistence so that it resets to the default position every time
    m_io->IniFilename = nullptr;

    ImGui_ImplGlfw_InitForOpenGL(window.getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 430");

    SetupStyle();
}

UISystem::~UISystem() {
    // cleaning up the openGL buffers and shaders (VBOs, EBOs, etc)
    ImGui_ImplOpenGL3_Shutdown();
    // cleaning up the glfw callbacks
    ImGui_ImplGlfw_Shutdown();
    // cleaning up the imgui context
    ImGui::DestroyContext();
}

void UISystem::NewFrame() {
    // checking if theGPU resources are valid (for OpenGL does almost nothing)
    ImGui_ImplOpenGL3_NewFrame();
    // reads mouse position, key presses, deltatime, etc from GLFW
    ImGui_ImplGlfw_NewFrame();
    // locks in the inputs and clears the draw list full of vertices etc
    ImGui::NewFrame();
}

bool UISystem::WantCaptureMouse() const {
    return m_io->WantCaptureMouse;
}

bool UISystem::WantCaptureKeyboard() const {
    return m_io->WantCaptureKeyboard;
}

void UISystem::CreateWindows() {
    if (m_pinn_view) {
        ImGui::SetNextWindowPos(ImVec2(5, 5));
        ImGui::SetNextWindowBgAlpha(0.5f);
        ImGui::Begin("##pinn_overlay", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

        // TODO we also need to lock the actual save button until the render is done for this frame, since we dont want half-baked renders
        if (ImGui::Button("Save")) {
            // TODO
        }
        ImGui::SameLine();
        if (ImGui::Button("Back")) {
            m_pinn_view = false;

        }
        ImGui::End();
        return;
    }

    // we make a simple little hint header to show the user they can open the settings menu with tab
    if (!m_settings_expanded) {
        ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowBgAlpha(0.5f);
        ImGui::Begin("##hint", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoInputs);
        ImGui::TextDisabled("Tab - Settings");
        ImGui::End();
        return;
    }

    ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSizeConstraints(ImVec2(320, 0), ImVec2(320, FLT_MAX));
    ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

    // ===== BLACK HOLE =====
    if (ImGui::CollapsingHeader("Black Hole")) {
        double mass = m_settings.GetBlackHoleMass();
        if (ImGui::DragScalar("Mass (kg)", ImGuiDataType_Double, &mass, 1e28f, nullptr, nullptr, "%.3e")) {
            m_settings.SetBlackHoleMass(mass);
        }

        ImGui::Text("Schwarzschild Radius: %.2f m", m_settings.GetRealRsMeters());
    }

    // ===== ACCRETION DISK =====
    if (ImGui::CollapsingHeader("Accretion Disk")) {
        float radii[2] = {m_settings.GetDiskInnerRadiusRatio(), m_settings.GetDiskOuterRadiusRatio()};
        if (ImGui::DragFloat2("Radius (Rs)", radii, 0.1f, 1.5f, 20.0f)) {
            m_settings.SetDiskRadii(radii[0], radii[1]);
        }

        float height = m_settings.GetDiskHeightRatio();
        if (ImGui::DragFloat("Height (Rs)", &height, 0.01f, 0.01f, 2.0f)) {
            m_settings.SetDiskHeight(height);
        }

        const char* modes[] = {"Flat", "Angle-Based", "Raymarched"};
        int mode = static_cast<int>(m_settings.GetDiskRenderMode());
        if (ImGui::Combo("Render Mode", &mode, modes, IM_ARRAYSIZE(modes))) {
            m_settings.SetDiskRenderMode(static_cast<unsigned int>(mode));
        }

        float absorption = m_settings.GetDiskAbsorptionCoeff();
        if (ImGui::SliderFloat("Absorption", &absorption, 0.0f, 5.0f)) {
            m_settings.SetDiskAbsorptionCoefficient(absorption);
        }

        bool useNoise = m_settings.GetDiskUseNoise();
        if (ImGui::Checkbox("Animated Texture", &useNoise)) {
            m_settings.SetDiskUseNoise(useNoise);
        }

        ImGui::Spacing();
        ImGui::TextDisabled("Colors");

        glm::vec4 colorHot = m_settings.GetDiskColorHot();
        glm::vec4 colorCool = m_settings.GetDiskColorCool();
        bool colorChanged = false;
        colorChanged |= ImGui::ColorEdit3("Hot", glm::value_ptr(colorHot));
        colorChanged |= ImGui::ColorEdit3("Cool", glm::value_ptr(colorCool));
        if (colorChanged) {
            m_settings.SetDiskColors(colorCool, colorHot);
        }
    }

    // ===== RADIAL MESH =====
    if (ImGui::CollapsingHeader("Spacetime Grid")) {
        bool enabled = m_settings.IsRadialMeshEnabled();
        if (ImGui::Checkbox("Show Grid", &enabled)) {
            m_settings.SetRadialMeshEnabled(enabled);
        }

        if (enabled) {
            int resolution[2] = {m_settings.GetRadialMeshRings(), m_settings.GetRadialMeshSpokes()};
            if (ImGui::SliderInt2("Rings / Spokes", resolution, 2, 255)) {
                m_settings.SetRadialMeshResolution(resolution[0], resolution[1]);
            }

            glm::vec4 color = m_settings.GetRadialMeshColor();
            if (ImGui::ColorEdit4("Color", glm::value_ptr(color))) {
                m_settings.SetRadialMeshColor(color);
            }
        }
    }

    // ===== CAMERA =====
    if (ImGui::CollapsingHeader("Camera")) {
        float orbit = m_settings.GetOrbitSensitivity();
        float zoom = m_settings.GetZoomSensitivity();

        if (ImGui::SliderFloat("Orbit Sensitivity", &orbit, 0.00001f, 0.001f, "%.5f")) {
            m_settings.SetOrbitSensitivity(orbit);
        }
        if (ImGui::SliderFloat("Zoom Sensitivity", &zoom, 0.001f, 0.05f, "%.4f")) {
            m_settings.SetZoomSensitivity(zoom);
        }
    }

    // ===== PERFORMANCE =====
    if (ImGui::CollapsingHeader("Performance")) {
        int maxSteps = m_settings.GetMaxRaySteps();
        if (ImGui::SliderInt("Max Ray Steps", &maxSteps, 50, 2000)) {
            m_settings.SetMaxRaySteps(maxSteps);
        }

        float stepSize = m_settings.GetRayStepSize();
        if (ImGui::SliderFloat("Ray Step Size", &stepSize, 0.01f, 1.0f)) {
            m_settings.SetRayStepSize(stepSize);
        }

        float ambient = m_settings.GetPlanetAmbientLight();
        if (ImGui::SliderFloat("Planet Ambient", &ambient, 0.0f, 0.5f)) {
            m_settings.SetPlanetAmbientLight(ambient);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("%.1f FPS (%.2f ms)", m_io->Framerate, 1000.0f / m_io->Framerate);
    }

    ImGui::Spacing();
    ImGui::Separator();
    if (ImGui::Button("Render with PINN", ImVec2(-1, 0))) {
        m_pinn_view = true;
        // TODO
    }

    ImGui::End();
}

void UISystem::Draw() {
    // calculates the triangle positions, colors etc and makes a list to send to the GPU
    ImGui::Render();
    // takes the rendering list and actually sends it to the GPU for rendering
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UISystem::SetupStyle() {
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();

    // rounded corners
    style.WindowRounding = 6.0f;
    style.FrameRounding = 4.0f;
    style.GrabRounding = 4.0f;

    style.WindowPadding = ImVec2(12, 12);
    style.FramePadding = ImVec2(8, 4);
    style.ItemSpacing = ImVec2(8, 6);

    // slight transparency
    style.Colors[ImGuiCol_WindowBg].w = 0.94f;

    // some nice accent color
    ImVec4 accent = ImVec4(0.26f, 0.59f, 0.98f, 1.0f);
    style.Colors[ImGuiCol_SliderGrab] = accent;
    style.Colors[ImGuiCol_CheckMark] = accent;
    style.Colors[ImGuiCol_Header] = ImVec4(accent.x, accent.y, accent.z, 0.4f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(accent.x, accent.y, accent.z, 0.6f);
}


void UISystem::ToggleKeyboardControls(bool enable) {
    // we flip the flag for keyboard controls so we can turn it off and on (So we can navigate the UI with keyboard whenever we expand it)
    if (enable) m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    else m_io->ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;
}

void UISystem::ToggleSettingsWindow() {
    m_settings_expanded = !m_settings_expanded;
    ToggleKeyboardControls(m_settings_expanded);
}

