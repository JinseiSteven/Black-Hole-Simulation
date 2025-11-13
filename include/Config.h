//
// Created by StephanVisser on 10/26/2025.
//

#ifndef BLACK_HOLE_SIMULATION_CONFIG_H
#define BLACK_HOLE_SIMULATION_CONFIG_H

#include <string>

namespace Config {
    // ==================== Base Application ====================
    const std::string TITLE{"Black Hole Simulation"};
    const std::string SHADER_PATH = "res/shaders/";
    const std::string COMPUTE_PATH = "res/compute/";
    constexpr int START_WIDTH = 800;
    constexpr int START_HEIGHT = 600;

    // ==================== Camera ====================
    // camera internal params to make the internal matrix
    constexpr float FOV_Y = 1.0f;
    constexpr float Z_NEAR = 0.1f;
    constexpr float Z_FAR = 20000.0f;
    constexpr float CAMERA_BASE_RADIUS = 1000.0f; // in su
    constexpr float BASE_CAMERA_ORBIT_SENSITIVITY = 0.0001f;
    constexpr float BASE_CAMERA_ZOOM_SENSITIVITY = 1.1f;

    // ==================== Physics Constants ====================
    constexpr double G_REAL = 6.67430e-11;            // Gravitational constant (m³/kg·s²)
    constexpr double C_REAL = 299792458.0;            // Speed of light (m/s)
    constexpr double SOLAR_MASS = 1.98892e30;         // Solar mass (kg)

    // some default black holes
    constexpr double SAGITTARIUS_A_STAR_MASS = 4.31e6 * SOLAR_MASS;  // Milky Way's supermassive BH
    constexpr double M87_STAR_MASS = 6.5e9 * SOLAR_MASS;             // M87 galaxy's supermassive BH

    // ==================== Simulation Space Definition ====================
    // this is the size of our simulation world in "sim units"
    constexpr float SIM_WORLD_RADIUS = 10000.0f;

    // this is the total distance our sim world radius represents (in m)
    constexpr double REAL_WORLD_RADIUS_METERS = 1.0e14;

    // ==================== Default Simulation Parameters ====================
    // black hole mass to generate usually, we just use the saggitatius one as default rn (Milky Ways supermassive black hole)
    constexpr double DEFAULT_BLACK_HOLE_MASS = SAGITTARIUS_A_STAR_MASS;

    // radial mesh params
    constexpr int DEFAULT_RADIAL_MESH_RINGS = 30;
    constexpr int DEFAULT_RADIAL_MESH_SPOKES = 36;
    constexpr int RADIAL_MESH_INNER_MARGIN = 1;    // in su
    constexpr int RADIAL_MESH_OUTER_MARGIN = 100;    // in su
    constexpr bool DEFAULT_RADIAL_MESH_ENABLE = true;
    constexpr float BASE_RADIAL_MESH_OPACITY = 0.5f;

    // the amount of steps the rays for the raytracer will take
    constexpr int DEFAULT_MAX_RAY_STEPS = 10000;
    constexpr float DEFAULT_RAY_STEP_SIZE = 0.1f; // in su

    // ==================== Rendering ====================
    // decides in what resolution we show the simulation (so we can upscale? Like, if its 2 then 4 pixels use 1 ray)
    constexpr float RENDER_SCALE = 1.0f;

    // workgroup sizes for the raytracer compute shader
    constexpr int WORKGROUP_SIZE_X = 8;
    constexpr int WORKGROUP_SIZE_Y = 8;

    // shader paths
    const std::string SCREENQUAD_VERT = SHADER_PATH + "screenQuad.vert";
    const std::string SCREENQUAD_FRAG = SHADER_PATH + "screenQuad.frag";
    const std::string RADIALMESH_VERT = SHADER_PATH + "radialMesh.vert";
    const std::string RADIALMESH_FRAG = SHADER_PATH + "radialMesh.frag";
    const std::string COMPUTE_SIM = COMPUTE_PATH + "simulation.comp";

    // ==================== Debug/UI ====================
    // constexpr bool ENABLE_DEBUG_UI = true;   // TODO soon
    constexpr float DEFAULT_GRID_OPACITY = 0.5f;
    constexpr bool DEFAULT_GRID_ENABLED = true;
}

#endif //BLACK_HOLE_SIMULATION_CONFIG_H