//
// Created by StephanVisser on 10/26/2025.
//

#ifndef BLACK_HOLE_SIMULATION_SIMULATION_H
#define BLACK_HOLE_SIMULATION_SIMULATION_H

#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>

class ComputeShader;
class Camera;
class Settings;


struct CameraUniforms {
    glm::mat4 invProjectionMatrix;
    glm::mat4 invViewMatrix;
    glm::vec4 camPosCartesian;  // padding after vec3's, due to std140 requirements (see notes)
};

struct SimSettingsUniforms {
    int maxStepCount;
    float stepSize;
    float Rs;
    float worldRadius;
};

struct DiskSettingsUniforms {
    float innerRadius;
    float outerRadius;
    float minHeight;
    float maxHeight;
    unsigned int renderMode;
    float absorptionCoeff;
    unsigned int maxMarchSteps;
    float marchStepSize;
    glm::vec4 colorHot;
    glm::vec4 colorCool;
};

struct PlanetUniforms {
    int numPlanets;
    float _pad[3];      // alignment padding again
    glm::vec4 planets[64];    // max of 64 planets (no way I need more)
};

class Simulation {
public:
    Simulation(
        const std::shared_ptr<Settings>& settings,
        unsigned int out_width,
        unsigned int out_height,
        unsigned int textureID,
        const char* computePath);
    ~Simulation();

    // stepping function which will calculate the next step in the simulation
    void step(const Camera* camera);

    void UpdatePlanetsData(const std::vector<glm::vec4>& planetData) const;
    [[nodiscard]] std::vector<float>& GetRadialHeightMap() const;

private:
    const std::shared_ptr<Settings> m_settings;
    unsigned int m_lastSimVersion{0xFFFFFFFF};
    unsigned int m_lastDiskVersion{0xFFFFFFFF};

    const unsigned int out_width, out_height;

    // vector representing the height map of the perspective grid (row-major)
    mutable std::vector<float> m_radial_height_map;
    mutable bool m_height_map_dirty{true};

    // scientific variables
    mutable float m_Rs{0};
    mutable bool m_Rs_dirty{true};

    std::unique_ptr<ComputeShader> compute_shader;
    unsigned int output_texture_id;

    unsigned int cameraUBO{0}, simUBO{0}, diskUBO{0}, planetUBO{0};

    void initUniformBuffers();
    void UpdateRadialHeightMap() const;
};


#endif //BLACK_HOLE_SIMULATION_SIMULATION_H