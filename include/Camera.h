//
// Created by StephanVisser on 10/27/2025.
//

#ifndef BLACK_HOLE_SIMULATION_CAMERA_H
#define BLACK_HOLE_SIMULATION_CAMERA_H

#include <glm/glm.hpp>

class Settings;


class Camera {
public:
    Camera(
        float fovY,
        float aspect,
        float zNear,
        float zFar);

    void SetPosition(const glm::vec3& position);
    void SetPositionSpherical(const glm::vec3& position_spherical);
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

    float m_fovY;
    float m_aspect;
    float m_zNear;
    float m_zFar;

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
    void UpdatePositionCartesian();
    void setOrthonormalBases();
    void UpdateInvViewMatrix() const;
    void UpdateInvProjectionMatrix() const;
};

#endif //BLACK_HOLE_SIMULATION_CAMERA_H