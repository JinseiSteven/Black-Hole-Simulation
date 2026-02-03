//
// Created by StephanVisser on 1/20/2026.
//

#include <algorithm>

#include "CameraController.h"
#include "Camera.h"
#include "InputState.h"
#include "Config.h"
#include "Settings.h"

CameraController::CameraController(Camera& camera, const Settings& settings) :
    m_camera(camera),
    m_settings(settings),
    m_orbit_sensitivity{m_settings.GetOrbitSensitivity()},
    m_zoom_sensitivity{m_settings.GetZoomSensitivity()},
    m_min_zoom_distance{m_settings.GetSimRs() * Config::MIN_ZOOM_DISTANCE_RS},
    m_max_zoom_distance{m_settings.GetSimWorldRadius() * 1.2f},
    m_last_camera_version{m_settings.GetCameraVersion()} {
    UpdateFromSettings();
    MoveInstant(m_target_position.x, m_target_position.y, m_target_position.z);
}

void CameraController::ProcessInput(const InputState &input_state) {
    // Even denken, hoe kunnen we op de beste mogelijke manier de input processen?
    // Iig willen we het zo hebben:
    // 1. process input die update de target position etc
    // 2. update die zet de nieuwe camera positie dmv de exponential smoothing etc
    if (input_state.keys_held_mask & CTRL_FLAG) {
        UpdateRadialTarget(input_state.mouse_delta_y);
    }
    else {
        UpdateAngularTarget(input_state.mouse_delta_x, input_state.mouse_delta_y);
    }
}


void CameraController::Update(float delta_time) {
    if (m_last_camera_version != m_settings.GetCameraVersion()) {
        UpdateFromSettings();
    }
    MoveCameraPosition(delta_time);
}

void CameraController::UpdateAngularTarget(double delta_x, double delta_y) {
    m_target_position.z += -static_cast<float>(delta_x) * m_orbit_sensitivity;
    const float inclination_change = static_cast<float>(delta_y) * m_orbit_sensitivity;

    // need to clamp the maximum inclination angle to prevent both gimbal lock and overturning
    m_target_position.y = std::clamp(
        m_target_position.y + inclination_change,
        MIN_INCLINATION,
        MAX_INCLINATION);
}

void CameraController::UpdateRadialTarget(double delta_y) {
    m_target_position.x *= std::pow(m_zoom_exponential_factor, static_cast<float>(delta_y) * m_zoom_sensitivity);
    m_target_position.x = std::clamp(m_target_position.x, m_min_zoom_distance, m_max_zoom_distance);
}

// does the camera movement using exponential smoothing
void CameraController::MoveCameraPosition(float delta_time) {
    glm::vec3 current = m_camera.GetPositionSpherical();

    // need to check if we are not at some invalid position, since thisll make the log calculation crash our program
    if (current.x <= 0.0f) {
        m_camera.SetPositionSpherical(m_target_position);
        return;
    }

    float angular_factor = 1.0f - std::exp(-m_angular_damping * delta_time);
    float radial_factor = 1.0f - std::exp(-m_radial_damping * delta_time);

    glm::vec3 new_position;
    new_position.y = std::lerp(current.y, m_target_position.y, angular_factor);
    new_position.z = std::lerp(current.z, m_target_position.z, angular_factor);

    // using log-space interpolation for the zooming since it feels more natural
    float log_current = std::log(current.x);
    float log_target = std::log(m_target_position.x);
    new_position.x = std::exp(std::lerp(log_current, log_target, radial_factor));

    m_camera.SetPositionSpherical(new_position);
}

// simple helper function we can use to set the camera position initially
void CameraController::MoveInstant(const float radius, const float incline, const float azimuth) {
    m_target_position = glm::vec3(radius, incline, azimuth);
    m_camera.SetPositionSpherical(glm::vec3(radius, incline, azimuth));
}

void CameraController::UpdateFromSettings() {
    m_orbit_sensitivity = m_settings.GetOrbitSensitivity();
    m_zoom_sensitivity = m_settings.GetZoomSensitivity();
    m_min_zoom_distance = m_settings.GetSimRs() * Config::MIN_ZOOM_DISTANCE_RS;
    m_max_zoom_distance = m_settings.GetSimWorldRadius() * 1.2f;
    m_last_camera_version = m_settings.GetCameraVersion();
}


