//
// Created by StephanVisser on 10/26/2025.
//

#ifndef BLACK_HOLE_SIMULATION_ENGINE_H
#define BLACK_HOLE_SIMULATION_ENGINE_H

#include <memory>
#include <GLFW/glfw3.h>


// forward declaration
class Settings;
class Renderer;
class InputHandler;
class Simulation;
class Camera;

class RayTraceEngine {
    GLFWwindow* window{nullptr};

    // some constants
    static constexpr int BASE_SCREEN_WIDTH = 800;
    static constexpr int BASE_SCREEN_HEIGHT = 600;

    unsigned int screen_width{BASE_SCREEN_WIDTH}, screen_height{BASE_SCREEN_HEIGHT};

    std::shared_ptr<Settings> settings;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<InputHandler> input_handler;
    std::unique_ptr<Simulation> simulation;
    std::unique_ptr<Renderer> renderer;

    void GL_init();
    [[nodiscard]] bool check_render_scale(float render_scale) const;
public:
    RayTraceEngine();
    RayTraceEngine(int width, int height);
    ~RayTraceEngine();

    // executes a single simulation step (might be private later)
    void step();

    // calculates how many simulation steps we need to advance, based on the amount of time that has passed
    void tick(double deltaTime);

    [[nodiscard]] unsigned int GetScreenWidth() const { return screen_width; }
    [[nodiscard]] unsigned int GetScreenHeight() const { return screen_height; }
    [[nodiscard]] GLFWwindow* GetWindow() const { return window; }
    [[nodiscard]] Camera* GetCamera() const { return camera.get(); }
    [[nodiscard]] InputHandler* GetInputHandler() const { return input_handler.get(); }
    [[nodiscard]] Simulation* GetSimulation() const { return simulation.get(); }
    [[nodiscard]] Renderer* GetRenderer() const { return renderer.get(); }
};

#endif //BLACK_HOLE_SIMULATION_ENGINE_H