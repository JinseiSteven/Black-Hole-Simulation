//
// Created by StephanVisser on 1/20/2026.
//

#ifndef BLACK_HOLE_SIMULATION_CAMERACONTROLLER_H
#define BLACK_HOLE_SIMULATION_CAMERACONTROLLER_H
#include <glm/vec3.hpp>
#include <glm/gtc/constants.hpp>

#include "Config.h"

class Camera;
class Settings;
struct InputState;

class CameraController {
public:
    CameraController(Camera& camera, const Settings& settings);
    void ProcessInput(const InputState& input_state);
    void Update(float delta_time);
    void MoveInstant(float radius, float incline, float azimuth);
private:
    Camera& m_camera;
    const Settings& m_settings;

    unsigned int m_last_camera_version{0xFFFFFFFF};

    float m_orbit_sensitivity;
    float m_zoom_sensitivity;
    float m_min_zoom_distance;
    float m_max_zoom_distance;

    // kind of arbitrary now, should probably move to settings
    float m_zoom_exponential_factor{1.1f};

    // radius, incline, azimuth
    glm::vec3 m_target_position{Config::CAMERA_BASE_RADIUS, glm::half_pi<float>(), glm::half_pi<float>()};

    float m_radial_damping{10.0f};
    float m_angular_damping{10.0f};

    float m_radial_velocity{0.0f};
    float m_angular_velocity{0.0f};

    void UpdateRadialTarget(double delta_y);
    void UpdateAngularTarget(double delta_x, double delta_y);

    void MoveCameraPosition(float delta_time);

    void UpdateFromSettings();

    // cant get it all the way exactly to 90 degrees or well get gimbal lock (yaw and roll will be the same)
    static constexpr float MIN_INCLINATION = 0.001f;
    static constexpr float MAX_INCLINATION = glm::pi<float>() - 0.001f;
};


#endif //BLACK_HOLE_SIMULATION_CAMERACONTROLLER_H