//
// Created by StephanVisser on 10/26/2025.
//

#ifndef BLACK_HOLE_SIMULATION_SIMULATION_H
#define BLACK_HOLE_SIMULATION_SIMULATION_H

#pragma once

#include <map>
#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include "Utils.h"

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
    float planetAmbientLight;
};

struct DiskSettingsUniforms {
    float innerRadius;
    float outerRadius;
    float height;
    unsigned int renderMode;
    float absorptionCoeff;
    unsigned int maxMarchSteps;
    float marchStepSize;
    int useNoise;  // bool as int for std140 alignment
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
        const Settings& settings,
        unsigned int out_width,
        unsigned int out_height,
        unsigned int textureID,
        const char* raytracePath,
        const char* pinnPath);
    ~Simulation();

    // stepping function which will calculate the next step in the simulation
    void step(const Camera& camera);

    void RenderPinnRows(const Camera& camera, unsigned int row_offset, unsigned int row_count);
    void ClearOutputTexture();

    [[nodiscard]] unsigned int GetRenderWidth() const { return out_width; }
    [[nodiscard]] unsigned int GetRenderHeight() const { return out_height; }

    void UpdatePlanetsData(const std::vector<Utils::PlanetData>& planetData) const;
    void UpdateRadialHeightMap(int num_rings, float inner_radius, float outer_radius);
    [[nodiscard]] const std::vector<float>& GetRadialHeightMap() const { return m_radial_height_map; }

private:
    const Settings& m_settings;
    unsigned int m_lastSimVersion{0xFFFFFFFF};
    unsigned int m_lastDiskVersion{0xFFFFFFFF};

    const unsigned int out_width, out_height;

    // vector representing the height map of the perspective grid (row-major)
    std::vector<float> m_radial_height_map;

    std::unique_ptr<ComputeShader> m_raytrace_shader;
    std::unique_ptr<ComputeShader> m_pinn_shader;
    unsigned int output_texture_id;

    unsigned int cameraUBO{0}, simUBO{0}, diskUBO{0}, planetUBO{0};

    std::map<std::string, int> m_textureMap;
    unsigned int planet_texture_array_id{0};
    unsigned int noise_texture_id{0};

    void initUniformBuffers();
    void initTextureArray(int width, int height, int depth);
    void initPlanetTextures();
    void initNoiseTexture();

    void updateCameraUBO(const Camera& camera);
    void updateSimUBO();
    void updateDiskUBO();
    void bindOutputTexture();
    void dispatch(unsigned int row_count);
};


#endif //BLACK_HOLE_SIMULATION_SIMULATION_H