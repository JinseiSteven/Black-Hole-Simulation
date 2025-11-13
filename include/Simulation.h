//
// Created by StephanVisser on 10/26/2025.
//

#ifndef BLACK_HOLE_SIMULATION_SIMULATION_H
#define BLACK_HOLE_SIMULATION_SIMULATION_H

#include <memory>
#include <vector>

class ComputeShader;
class Camera;
class Settings;


class Simulation {
public:
    Simulation(
        const std::shared_ptr<Settings>& settings,
        unsigned int out_width,
        unsigned int out_height,
        unsigned int textureID,
        const char* computePath);
    ~Simulation() = default;

    // stepping function which will calculate the next step in the simulation
    void step(const Camera* camera);

    [[nodiscard]] std::vector<float>& GetRadialHeightMap() const;

private:
    const std::shared_ptr<Settings> m_settings;
    const unsigned int out_width, out_height;

    // vector representing the height map of the perspective grid (row-major)
    mutable std::vector<float> m_radial_height_map;
    mutable bool m_height_map_dirty{true};

    // scientific variables
    mutable float m_Rs{0};
    mutable bool m_Rs_dirty{true};

    std::unique_ptr<ComputeShader> compute_shader;
    unsigned int output_texture_id;

    void UpdateRadialHeightMap() const;
};


#endif //BLACK_HOLE_SIMULATION_SIMULATION_H