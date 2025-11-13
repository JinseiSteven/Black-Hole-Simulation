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
}

void Simulation::step(const Camera* camera) {
    // setting all the planets -> sending to the GPU

    compute_shader->use();

    // TODO, will be replacing this with the combined matrix
    compute_shader->SetMat4("invProjectionMatrix", camera->GetInvProjectionMatrix());
    compute_shader->SetMat4("invViewMatrix", camera->GetInvViewMatrix());
    compute_shader->SetVec3("camPos", camera->GetPosition());

    compute_shader->SetInt("maxStepCount", m_settings->GetMaxRaySteps());
    compute_shader->SetFloat("stepSize", m_settings->GetRayStepSize());

    compute_shader->SetFloat("Rs", m_settings->GetSimRs());
    compute_shader->SetFloat("worldRadius", m_settings->GetSimWorldRadius());

    glBindImageTexture(
        0,                 // binding = 0
        output_texture_id, // texture ID
        0,                 // mipmap level (0 = base)
        GL_FALSE,          // 3D/array texture? No
        0,                 // if so, what layer?
        GL_WRITE_ONLY,     // access
        GL_RGBA8           // format (rgba8 same as in shader)
    );

    const GLuint numGroupsX = (out_width + Config::WORKGROUP_SIZE_X - 1) / Config::WORKGROUP_SIZE_X;
    const GLuint numGroupsY = (out_height + Config::WORKGROUP_SIZE_Y - 1) / Config::WORKGROUP_SIZE_Y;

    glDispatchCompute(numGroupsX, numGroupsY, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

std::vector<float>& Simulation::GetRadialHeightMap() const {
    if (m_height_map_dirty) {
        UpdateRadialHeightMap();
        m_height_map_dirty = false;
    }
    return m_radial_height_map;
}

void Simulation::UpdateRadialHeightMap() const {
    // we weten de x en z omdat de map een bepaalde ruimte spant, dus we verdelen die ruimte gewoon over de grid-cells

    for (int r = 0; r < m_settings->GetRadialMeshRings(); r++) {
        m_radial_height_map[r] = 0.0f;  // TODO eerst maar even op 0.0 gewoon, later berekenen we de warping wel
    }
}
