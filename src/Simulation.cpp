//
// Created by StephanVisser on 10/26/2025.
//


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream> // === PINN WEIGHTS ===

#include "Simulation.h"
#include "Camera.h"
#include "ComputeShader.h"
#include "Config.h"
#include "Settings.h"
#include "stb_image.h"


Simulation::Simulation(
    const Settings& settings,
    const unsigned int out_width,
    const unsigned int out_height,
    const unsigned int textureID,
    const char* raytracePath,
    const char* pinnPath) :
    m_settings(settings),
    out_width(out_width),
    out_height(out_height),
    m_raytrace_shader(std::make_unique<ComputeShader>(raytracePath)),
    output_texture_id(textureID) {
    const int rings = m_settings.GetRadialMeshRings();
    m_radial_height_map.resize(rings);

    initUniformBuffers();

    // === DEBUG: trace pinn init ===
    std::cout << "Compiling pinn shader..." << std::flush;
    m_pinn_shader = std::make_unique<ComputeShader>(pinnPath);
    std::cout << " OK" << std::endl;
    // === END DEBUG ===

    initPlanetTextures();
    initNoiseTexture();
    initPinnWeights();
}

Simulation::~Simulation() {
    glDeleteBuffers(1, &cameraUBO);
    glDeleteBuffers(1, &simUBO);
    glDeleteBuffers(1, &diskUBO);
    // === PINN WEIGHTS ===
    if (farWeightsSSBO) glDeleteBuffers(1, &farWeightsSSBO);
    if (nearWeightsSSBO) glDeleteBuffers(1, &nearWeightsSSBO);
    // === END PINN WEIGHTS ===
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

void Simulation::initTextureArray(const int width, const int height, const int depth) {
    if (planet_texture_array_id == 0) {
        glGenTextures(1, &planet_texture_array_id);
    }

    glBindTexture(GL_TEXTURE_2D_ARRAY, planet_texture_array_id);

    glTexImage3D(
        GL_TEXTURE_2D_ARRAY,
        0,
        GL_RGBA8,
        width,
        height,
        depth,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr
    );

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Simulation::initPlanetTextures() {
    const std::vector<std::string> files = Utils::getFilesInDirectory(Config::PLANET_TEXTURE_PATH, ".png");
    const int texture_count = std::min(static_cast<int>(files.size()), Config::MAX_TEXTURE_COUNT);

    initTextureArray(
        Config::PLANET_TEXTURE_WIDTH,
        Config::PLANET_TEXTURE_HEIGHT,
        texture_count
    );

    int layer_index = 0;

    for (int i = 0; i < texture_count; i++) {
        const std::string& texName = files[i];

        if (layer_index >= texture_count) break;

        // only want to add all textures once (should only be possible once due to naming in folders, but oh well)
        if (m_textureMap.contains(texName)) {
            continue;
        }

        int width, height, nr_components;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load((
            Config::PLANET_TEXTURE_PATH + texName).c_str(),
            &width, &height,
            &nr_components,
            STBI_rgb_alpha);

        if (width != Config::PLANET_TEXTURE_WIDTH || height != Config::PLANET_TEXTURE_HEIGHT) {
            std::cerr << "Error: Texture size mismatch (" << Config::PLANET_TEXTURE_PATH + texName << ")."
              << " Found " << width << "x" << height
              << ", expected " << Config::PLANET_TEXTURE_WIDTH
              << "x" << Config::PLANET_TEXTURE_HEIGHT
              << std::endl;
            if (data) stbi_image_free(data);
            continue;
        }

        if (data) {
            glTexSubImage3D(
                GL_TEXTURE_2D_ARRAY,
                0,
                0, 0, layer_index,
                width, height, 1,
                GL_RGBA, GL_UNSIGNED_BYTE,
                data
            );
            stbi_image_free(data);
            m_textureMap.insert({texName, layer_index});
            layer_index++;
        }
        else {
            std::cout << "No image data found inside of " << texName << std::endl;
        }
    }

    m_raytrace_shader->use();

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D_ARRAY, planet_texture_array_id);
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

    // like before, the int we set tells the shader what texture slot we have loaded our sampler data
    m_raytrace_shader->SetInt("planetTextureArray", 1);

    // === PINN WEIGHTS: bind same textures to pinn shader ===
    m_pinn_shader->use();
    m_pinn_shader->SetInt("planetTextureArray", 1);
    // === END ===
}

void Simulation::initNoiseTexture() {
    int width, height, nr_components;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(
        Config::DISK_NOISE_PATH.c_str(),
        &width, &height,
        &nr_components,
        STBI_grey);

    if (!data) {
        std::cout << "No image data found for noise texture inside of " << Config::DISK_NOISE_PATH << std::endl;
        return;
    }

    if (noise_texture_id == 0) {
        glGenTextures(1, &noise_texture_id);
    }

    m_raytrace_shader->use();
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, noise_texture_id);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_R8,
        width,
        height,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        data
    );
    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    m_raytrace_shader->SetInt("noiseTexture", 2);

    // === PINN WEIGHTS: bind same textures to pinn shader ===
    m_pinn_shader->use();
    m_pinn_shader->SetInt("noiseTexture", 2);
    // === END ===
}

void Simulation::updateCameraUBO(const Camera& camera) {
    const CameraUniforms cameraData = {
        .invProjectionMatrix = camera.GetInvProjectionMatrix(),
        .invViewMatrix = camera.GetInvViewMatrix(),
        .camPosCartesian = glm::vec4(camera.GetPosition(), 0.0f),
    };
    glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraUniforms), &cameraData);
}

void Simulation::bindOutputTexture() {
    glBindImageTexture(
        0,                 // binding = 0
        output_texture_id, // texture ID
        0,                 // mipmap level (0 = base)
        GL_FALSE,          // 3D/array texture? No
        0,                 // if so, what layer?
        GL_WRITE_ONLY,     // access
        GL_RGBA8           // data format
    );
}

void Simulation::dispatch(unsigned int row_count) {
    const GLuint numGroupsX = (out_width + Config::WORKGROUP_SIZE_X - 1) / Config::WORKGROUP_SIZE_X;
    const GLuint numGroupsY = (row_count + Config::WORKGROUP_SIZE_Y - 1) / Config::WORKGROUP_SIZE_Y;

    glDispatchCompute(numGroupsX, numGroupsY, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void Simulation::updateSimUBO() {
    if (m_settings.GetSimulationVersion() != m_lastSimVersion) {
        const SimSettingsUniforms simData = {
            .maxStepCount = m_settings.GetMaxRaySteps(),
            .stepSize = m_settings.GetRayStepSize(),
            .Rs = m_settings.GetSimRs(),
            .worldRadius = m_settings.GetSimWorldRadius(),
            .planetAmbientLight = m_settings.GetPlanetAmbientLight()
        };
        glBindBuffer(GL_UNIFORM_BUFFER, simUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SimSettingsUniforms), &simData);
        m_lastSimVersion = m_settings.GetSimulationVersion();
    }
}

void Simulation::updateDiskUBO() {
    if (m_settings.GetDiskVersion() != m_lastDiskVersion) {
        const DiskSettingsUniforms diskData = {
            .innerRadius = m_settings.GetDiskInnerRadius(),
            .outerRadius = m_settings.GetDiskOuterRadius(),
            .height = m_settings.GetDiskHeight(),
            .renderMode = m_settings.GetDiskRenderMode(),
            .absorptionCoeff = m_settings.GetDiskAbsorptionCoeff(),
            .maxMarchSteps = m_settings.GetDiskMaxMarchSteps(),
            .marchStepSize = m_settings.GetDiskMarchStepSize(),
            .useNoise = m_settings.GetDiskUseNoise() ? 1 : 0,
            .colorHot = m_settings.GetDiskColorHot(),
            .colorCool = m_settings.GetDiskColorCool(),
        };
        glBindBuffer(GL_UNIFORM_BUFFER, diskUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(DiskSettingsUniforms), &diskData);
        m_lastDiskVersion = m_settings.GetDiskVersion();
    }
}

// === PINN WEIGHTS: load binary weight files into SSBOs ===
static bool loadWeightsFile(const std::string& path, unsigned int& ssbo, unsigned int binding,
                            float& pos_scale, float& lmbda_scale, const std::string& label) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "PINN " << label << " weights not found at " << path << std::endl;
        return false;
    }

    int hidden_dim, layer_count, L;
    file.read(reinterpret_cast<char*>(&hidden_dim), 4);
    file.read(reinterpret_cast<char*>(&layer_count), 4);
    file.read(reinterpret_cast<char*>(&L), 4);
    file.read(reinterpret_cast<char*>(&pos_scale), 4);
    file.read(reinterpret_cast<char*>(&lmbda_scale), 4);

    int input_dim = 6 + 6 * 2 * L + 1;
    int total_floats =
        hidden_dim * input_dim + hidden_dim +
        layer_count * (hidden_dim * hidden_dim + hidden_dim) +
        3 * hidden_dim + 3;

    std::vector<float> weights(total_floats);
    file.read(reinterpret_cast<char*>(weights.data()), total_floats * sizeof(float));
    file.close();

    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, total_floats * sizeof(float), weights.data(), GL_STATIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, ssbo);

    std::cout << "PINN " << label << " loaded: hidden=" << hidden_dim
              << " layers=" << layer_count
              << " pos_scale=" << pos_scale
              << " (" << total_floats * 4 / 1024 << " KB)" << std::endl;
    return true;
}

void Simulation::initPinnWeights() {
    bool far_ok = loadWeightsFile(Config::PINN_FAR_WEIGHTS_PATH, farWeightsSSBO, 5,
                                   far_pos_scale, far_lmbda_scale, "far");
    bool near_ok = loadWeightsFile(Config::PINN_NEAR_WEIGHTS_PATH, nearWeightsSSBO, 6,
                                    near_pos_scale, near_lmbda_scale, "near");
    pinn_weights_loaded = far_ok && near_ok;
    if (!pinn_weights_loaded) {
        std::cout << "PINN rendering disabled (missing weight files)" << std::endl;
    }
}
// === END PINN WEIGHTS ===

void Simulation::step(const Camera& camera) {
    m_raytrace_shader->use();

    m_raytrace_shader->SetFloat("time", static_cast<float>(glfwGetTime()));

    updateCameraUBO(camera);
    updateSimUBO();
    updateDiskUBO();

    bindOutputTexture();
    dispatch(out_height);

    // === DEBUG ===
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cout << "GL ERROR after step: " << err << std::endl;
    }
    // === END DEBUG ===
}

void Simulation::RenderPinnRows(const Camera& camera, unsigned int row_offset, unsigned int row_count) {
    if (!pinn_weights_loaded) return;

    // so now we need to dispatch the GPU computation for the different rows of the output image, starting from the row_offset
    m_pinn_shader->use();

    m_pinn_shader->SetInt("rowOffset", static_cast<int>(row_offset));

    // === PINN WEIGHTS: set dual-model uniforms ===
    m_pinn_shader->SetFloat("far_pos_scale", far_pos_scale);
    m_pinn_shader->SetFloat("far_lmbda_scale", far_lmbda_scale);
    m_pinn_shader->SetFloat("near_pos_scale", near_pos_scale);
    m_pinn_shader->SetFloat("near_lmbda_scale", near_lmbda_scale);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, farWeightsSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, nearWeightsSSBO);
    // === END PINN WEIGHTS ===

    updateCameraUBO(camera);
    updateSimUBO();
    updateDiskUBO();
    bindOutputTexture();
    dispatch(row_count);
}

void Simulation::ClearOutputTexture() {
    GLubyte clear[4] = {15, 15, 15, 255};
    glClearTexImage(output_texture_id, 0, GL_RGBA, GL_UNSIGNED_BYTE, clear);
}

void Simulation::UpdatePlanetsData(const std::vector<Utils::PlanetData>& planetData) const {
    PlanetUniforms data{};
    std::vector<int> textureIndexMap;
    data.numPlanets = std::min(static_cast<int>(planetData.size()), 64);

    for (int i = 0; i < data.numPlanets; i++) {

        int tex_index = 0;
        try {
            tex_index = m_textureMap.at(planetData[i].textureName);
        }
        catch (const std::out_of_range& e) {
            std::cout << "Failed to get texture during planet data update, falling back to default texture: " << e.what() << std::endl;
        }

        data.planets[i] = planetData[i].geometryData;
        textureIndexMap.push_back(tex_index);
    }

    m_raytrace_shader->use();

    glBindBuffer(GL_UNIFORM_BUFFER, planetUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PlanetUniforms), &data);

    m_raytrace_shader->Set1iv("textureIndexMap", textureIndexMap);

    // === PINN WEIGHTS: set same planet data on pinn shader ===
    m_pinn_shader->use();
    m_pinn_shader->Set1iv("textureIndexMap", textureIndexMap);
    // === END PINN WEIGHTS ===
}

void Simulation::UpdateRadialHeightMap(const int num_rings, const float inner_radius, const float outer_radius) {
    const float Rs = m_settings.GetSimRs();
    m_radial_height_map.resize(num_rings);

    float max_outer_height = 2.0f * std::sqrt(std::max(0.0f, Rs * (outer_radius - Rs)));

    for (int i = 0; i < num_rings; i++) {
        const float r_norm = (num_rings == 1) ? 0.0f : static_cast<float>(i) / static_cast<float>(num_rings - 1);
        const float r_world = glm::mix(inner_radius, outer_radius, r_norm);

        // clamping to prevent negative values due to vertex position jumping inside black hole
        const float squared_val = std::max(0.0f, Rs * (r_world - Rs));

        // minus sign in front in order to make it a well instead of a mountain
        m_radial_height_map[i] = 2.0f * std::sqrt(squared_val) - max_outer_height;
    }
}

