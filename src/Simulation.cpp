//
// Created by StephanVisser on 10/26/2025.
//


#include <GL/glew.h>

#include "Simulation.h"
#include "Camera.h"
#include "ComputeShader.h"
#include "Config.h"
#include <Settings.h>


Simulation::Simulation(
    const std::shared_ptr<Settings>& settings,
    const unsigned int out_width,
    const unsigned int out_height,
    const unsigned int textureID,
    const char* computePath) :
    m_settings(settings),
    out_width(out_width),
    out_height(out_height),
    compute_shader(std::make_unique<ComputeShader>(computePath)),
    output_texture_id(textureID) {
    // we want the amount of grids to be even
    const int rings = m_settings->GetRadialMeshRings();
    m_radial_height_map.resize(rings % 2 == 0 ? rings + 1 : rings);

    initUniformBuffers();
}

Simulation::~Simulation() {
    glDeleteBuffers(1, &cameraUBO);
    glDeleteBuffers(1, &simUBO);
    glDeleteBuffers(1, &diskUBO);
}

void Simulation::initUniformBuffers() {
    glGenBuffers(1, &cameraUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraUniforms), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, cameraUBO);

    glGenBuffers(1, &simUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, simUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(SimSettingsUniforms), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, simUBO);

    glGenBuffers(1, &diskUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, diskUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(DiskSettingsUniforms), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 3, diskUBO);

    glGenBuffers(1, &planetUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, planetUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PlanetUniforms), nullptr, GL_STATIC_DRAW);   // static for this one, not gonna add em during runtime
    glBindBufferBase(GL_UNIFORM_BUFFER, 4, planetUBO);
}

void Simulation::step(const Camera* camera) {
    compute_shader->use();

    const CameraUniforms cameraData = {
        .invProjectionMatrix = camera->GetInvProjectionMatrix(),
        .invViewMatrix = camera->GetInvViewMatrix(),
        .camPosCartesian = glm::vec4(camera->GetPosition(), 0.0f),
    };
    glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraUniforms), &cameraData);

    if (m_settings->GetSimulationVersion() != m_lastSimVersion) {
        const SimSettingsUniforms simData = {
            .maxStepCount = m_settings->GetMaxRaySteps(),
            .stepSize = m_settings->GetRayStepSize(),
            .Rs = m_settings->GetSimRs(),
            .worldRadius = m_settings->GetSimWorldRadius()
        };
        glBindBuffer(GL_UNIFORM_BUFFER, simUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SimSettingsUniforms), &simData);
        m_lastSimVersion = m_settings->GetSimulationVersion();
    }

    if (m_settings->GetDiskVersion() != m_lastDiskVersion) {
        const DiskSettingsUniforms diskData = {
            .innerRadius = m_settings->GetDiskInnerRadius(),
            .outerRadius = m_settings->GetDiskOuterRadius(),
            .minHeight = m_settings->GetDiskMinHeight(),
            .maxHeight = m_settings->GetDiskMaxHeight(),
            .renderMode = m_settings->GetDiskRenderMode(),
            .absorptionCoeff = m_settings->GetDiskAbsorptionCoeff(),
            .maxMarchSteps = m_settings->GetDiskMaxMarchSteps(),
            .marchStepSize = m_settings->GetDiskMarchStepSize(),
            .colorHot = m_settings->GetDiskColorHot(),
            .colorCool = m_settings->GetDiskColorCool(),
        };
        glBindBuffer(GL_UNIFORM_BUFFER, diskUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(DiskSettingsUniforms), &diskData);
        m_lastDiskVersion = m_settings->GetDiskVersion();
    }

    glBindImageTexture(
        0,                 // binding = 0
        output_texture_id, // texture ID
        0,                 // mipmap level (0 = base)
        GL_FALSE,          // 3D/array texture? No
        0,                 // if so, what layer?
        GL_WRITE_ONLY,     // access
        GL_RGBA8           // data format
    );

    const GLuint numGroupsX = (out_width + Config::WORKGROUP_SIZE_X - 1) / Config::WORKGROUP_SIZE_X;
    const GLuint numGroupsY = (out_height + Config::WORKGROUP_SIZE_Y - 1) / Config::WORKGROUP_SIZE_Y;

    glDispatchCompute(numGroupsX, numGroupsY, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void Simulation::UpdatePlanetsData(const std::vector<glm::vec4>& planetData) const {
    PlanetUniforms data{};
    data.numPlanets = std::min(static_cast<int>(planetData.size()), 64);

    for (int i = 0; i < data.numPlanets; i++) {
        data.planets[i] = planetData[i];
    }

    glBindBuffer(GL_UNIFORM_BUFFER, planetUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PlanetUniforms), &data);
}

std::vector<float>& Simulation::GetRadialHeightMap() const {
    if (m_height_map_dirty) {
        UpdateRadialHeightMap();
        m_height_map_dirty = false;
    }
    return m_radial_height_map;
}

void Simulation::UpdateRadialHeightMap() const {
    for (int r = 0; r < m_settings->GetRadialMeshRings(); r++) {
        m_radial_height_map[r] = 0.0f;  // TODO eerst maar even op 0.0 gewoon, later berekenen we de warping wel
    }
}
