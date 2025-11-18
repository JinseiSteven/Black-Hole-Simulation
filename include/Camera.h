//
// Created by StephanVisser on 10/27/2025.
//

#ifndef BLACK_HOLE_SIMULATION_CAMERA_H
#define BLACK_HOLE_SIMULATION_CAMERA_H

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

class Settings;

class Camera {
public:
    Camera(
        float fovY,
        float aspect,
        float zNear,
        float zFar,
        const std::shared_ptr<Settings>& settings
        );

    void ProcessZoom(float yoffset);
    void ProcessMouseMovement(float xoffset, float yoffset);

    void SetPosition(const glm::vec3& position);
    void LookAt(const glm::vec3& target);

    void SetAspectRatio(float aspect);
    void SetFovY(float fovY);

    const glm::mat4& GetInvViewMatrix() const;
    const glm::mat4& GetInvProjectionMatrix() const;
    const glm::mat4& GetInvViewProjectionMatrix() const;
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;
    glm::mat4 GetViewProjectionMatrix() const;
    const glm::vec3& GetPosition() const { return m_position; }
    const glm::vec3& GetPositionSpherical() const { return m_position_spherical; }

private:
    // cant get it all the way exactly to 90 degrees or well get gimbal lock (yaw and roll will be the same)
    static constexpr float MAX_INCLINATION = glm::half_pi<float>() * 0.999f;

    float m_fovY;
    float m_aspect;
    float m_zNear;
    float m_zFar;

    // kind of arbitrary initial parameters, especially the radius
    float m_azimuth{0.0f};
    float m_inclination{0.0f};
    float m_radius;

    // decouple into x, y and zoom sensitivities
    float m_orbit_sensitivity;
    float m_zoom_sensitivity;
    float m_min_zoom_distance;
    float m_max_zoom_distance;

    // we instantiate the camera to be at the origin facing the -z direction
    glm::vec3 m_position{0.0f, 0.0f, 0.0f};
    glm::vec3 m_position_spherical{0.0f, 0.0f, 0.0f};
    glm::vec3 m_target{0.0f, 0.0f, 0.0f};

    glm::vec3 m_forward{0.0f, 0.0f, -1.0f};
    glm::vec3 m_right{1.0f, 0.0f, 0.0f};
    glm::vec3 m_up{0.0f, 1.0f, 0.0f};

    mutable bool m_viewDirty;
    mutable bool m_projectionDirty;
    mutable bool m_viewProjectionDirty;

    mutable glm::mat4 m_invViewMatrix{glm::mat4(1.0f)};
    mutable glm::mat4 m_invProjectionMatrix{glm::mat4(1.0f)};
    mutable glm::mat4 m_invViewProjectionMatrix{glm::mat4(1.0f)};

    void UpdatePositionSpherical();
    void setOrthonormalBases();
    void UpdateInvViewMatrix() const;
    void UpdateInvProjectionMatrix() const;
};

#endif //BLACK_HOLE_SIMULATION_CAMERA_H