//
// Created by StephanVisser on 29-Jan-26.
//

#ifndef BLACK_HOLE_SIMULATION_UISYSTEM_H
#define BLACK_HOLE_SIMULATION_UISYSTEM_H

class Window;
class ImGuiIO;
class Settings;


class UISystem {
public:
    UISystem(const Window& window, Settings& settings);
    ~UISystem();

    void NewFrame();
    [[nodiscard]] bool WantCaptureMouse() const;
    [[nodiscard]] bool WantCaptureKeyboard() const;
    [[nodiscard]] bool IsPinnView() const { return m_pinn_view; }
    void CreateWindows();
    void ToggleSettingsWindow();

    void Draw();
private:
    Settings& m_settings;
    ImGuiIO* m_io{nullptr};
    bool m_settings_expanded{false};
    bool m_pinn_view{false};
    void SetupStyle();
    void ToggleKeyboardControls(bool enable);
};


#endif //BLACK_HOLE_SIMULATION_UISYSTEM_H