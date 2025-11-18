//
// Created by StephanVisser on 10/27/2025.
//

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <algorithm>
#include <memory>

#include "Camera.h"
#include "Settings.h"
#include "Config.h"
#include "Utils.h"


class Settings;

Camera::Camera(
        const float fovY,
        const float aspect,
        const float zNear,
        const float zFar,
        const std::shared_ptr<Settings>& settings
        ) :
        m_fovY(fovY),
        m_aspect(aspect),
        m_zNear(zNear),
        m_zFar(zFar),
        m_radius(Config::CAMERA_BASE_RADIUS)
{
        m_orbit_sensitivity = settings->GetOrbitSensitivity();
        m_zoom_sensitivity = settings->GetZoomSensitivity();
        m_min_zoom_distance = settings->GetSimRs() * 1.5f;
        m_max_zoom_distance = settings->GetSimWorldRadius() * 1.2f;

        // setting the initial position based on the radius and starting angles
        const float x = m_target.x + m_radius * std::cos(m_inclination) * std::sin(m_azimuth);
        const float y = m_target.y + m_radius * std::sin(m_inclination);
        const float z = m_target.z + m_radius * std::cos(m_inclination) * std::cos(m_azimuth);
        m_position = glm::vec3(x, y, z);

        UpdatePositionSpherical();
        setOrthonormalBases();
        UpdateInvProjectionMatrix();
        UpdateInvViewMatrix();

        m_viewDirty = false;
        m_projectionDirty = false;
        m_viewProjectionDirty = false;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset) {
        m_azimuth += xoffset * m_orbit_sensitivity;
        m_inclination += yoffset * m_orbit_sensitivity;
        m_inclination = std::clamp(m_inclination, -MAX_INCLINATION, MAX_INCLINATION);

        const float x = m_target.x + m_radius * std::cos(m_inclination) * std::sin(m_azimuth);
        const float y = m_target.y + m_radius * std::sin(m_inclination);
        const float z = m_target.z + m_radius * std::cos(m_inclination) * std::cos(m_azimuth);

        SetPosition(glm::vec3(x, y, z));
}

void Camera::ProcessZoom(float yoffset) {
        m_radius *= std::pow(1.1f, -yoffset * m_zoom_sensitivity);

        m_radius = std::clamp(m_radius, m_min_zoom_distance, m_max_zoom_distance);

        const float x = m_target.x + m_radius * std::cos(m_inclination) * std::sin(m_azimuth);
        const float y = m_target.y + m_radius * std::sin(m_inclination);
        const float z = m_target.z + m_radius * std::cos(m_inclination) * std::cos(m_azimuth);

        SetPosition(glm::vec3(x, y, z));
}

void Camera::SetPosition(const glm::vec3& position) {
        m_position = position;
        setOrthonormalBases();
        UpdatePositionSpherical();
        m_viewDirty = true;
        m_viewProjectionDirty = true;
}
void Camera::LookAt(const glm::vec3& target) {
        m_target = target;
        setOrthonormalBases();
        m_viewDirty = true;
        m_viewProjectionDirty = true;
}

void Camera::SetAspectRatio(const float aspect) {
        m_aspect = aspect;
        m_projectionDirty = true;
        m_viewProjectionDirty = true;
}

void Camera::SetFovY(const float fovY) {
        m_fovY = fovY;
        m_projectionDirty = true;
        m_viewProjectionDirty = true;
}

const glm::mat4& Camera::GetInvViewMatrix() const {
        if (m_viewDirty) {
                UpdateInvViewMatrix();
                m_viewDirty = false;
        }
        return m_invViewMatrix;
}

const glm::mat4& Camera::GetInvProjectionMatrix() const {
        if (m_projectionDirty) {
                UpdateInvProjectionMatrix();
                m_projectionDirty = false;
        }
        return m_invProjectionMatrix;
}

const glm::mat4& Camera::GetInvViewProjectionMatrix() const {
        if (m_viewProjectionDirty) {
                m_invViewProjectionMatrix = GetInvViewMatrix() * GetInvProjectionMatrix();
                m_viewProjectionDirty = false;
        }
        return m_invViewProjectionMatrix;
}

glm::mat4 Camera::GetViewMatrix() const {
        if (m_viewDirty) {
                UpdateInvViewMatrix();
                m_viewDirty = false;
        }
        return glm::inverse(m_invViewMatrix);
}

glm::mat4 Camera::GetProjectionMatrix() const {
        if (m_projectionDirty) {
                UpdateInvProjectionMatrix();
                m_projectionDirty = false;
        }
        return glm::inverse(m_invProjectionMatrix);
}

glm::mat4 Camera::GetViewProjectionMatrix() const {
        return glm::inverse(GetInvViewProjectionMatrix());
}

void Camera::UpdatePositionSpherical() {
        m_position_spherical = Utils::cartesianToSpherical(m_position);
}

void Camera::UpdateInvViewMatrix() const {
        const glm::mat4 translation = glm::mat4(
                1.0f,           0.0f,           0.0f,           0.0f,
                0.0f,           1.0f,           0.0f,           0.0f,
                0.0f,           0.0f,           1.0f,           0.0f,
                m_position.x,   m_position.y,   m_position.z,   1.0f);
        const glm::mat4 rotation = glm::mat4(
                m_right.x,      m_right.y,      m_right.z,      0.0f,
                m_up.x,         m_up.y,         m_up.z,         0.0f,
                -m_forward.x,   -m_forward.y,   -m_forward.z,   0.0f,
                0.0f,           0.0f,           0.0f,           1.0f);

        // also known as the rigidbody transformation for the camera
        m_invViewMatrix = translation * rotation;
}

void Camera::UpdateInvProjectionMatrix() const {
        // using the basic function for it because this one is a bit more of a pain lmao
        // might seem counter-intuitive to invert it, but we do this because we access the invProjMat way more
        m_invProjectionMatrix = glm::inverse(glm::perspective(m_fovY, m_aspect, m_zNear, m_zFar));
}

void Camera::setOrthonormalBases() {
        // using the Gram-Schmidt process to build our orthonormal basis for the camera
        m_forward = glm::normalize(m_target - m_position);
        m_right = glm::normalize(glm::cross(m_forward, glm::vec3(0.0f, 1.0f, 0.0f)));
        m_up = glm::normalize(glm::cross(m_right, m_forward));
}

