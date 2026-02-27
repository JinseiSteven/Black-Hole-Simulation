//
// Created by StephanVisser on 10/26/2025.
//

#ifndef BLACK_HOLE_SIMULATION_CONFIG_H
#define BLACK_HOLE_SIMULATION_CONFIG_H

#include <string>
#include <glm/glm.hpp>

namespace Config {
    // ==================== Base Application ====================
    const std::string TITLE{"Black Hole Simulation"};
    const std::string SHADER_PATH = "res/shaders/";
    const std::string COMPUTE_PATH = "res/compute/";
    const std::string PLANETS_PATH = "res/planets/";
    const std::string TEXTURE_PATH = "res/textures/";
    const std::string PLANET_TEXTURE_PATH = "res/textures/planets/";
    constexpr double FPS_UPDATE_LATENCY = 0.5;

    // ==================== Camera ====================
    // camera internal params to make the internal matrix
    constexpr float FOV_Y = 1.0f;
    constexpr float Z_NEAR = 0.1f;
    constexpr float Z_FAR = 20000.0f;
    constexpr float CAMERA_BASE_RADIUS = 300.0f; // in su
    constexpr float BASE_CAMERA_ORBIT_SENSITIVITY = 0.0001f;
    constexpr float BASE_CAMERA_ZOOM_SENSITIVITY = 0.005f;
    constexpr float MIN_ZOOM_DISTANCE_RS = 1.5f;  // minimum zoom distance in Schwarzschild radii

    // ==================== Physics Constants ====================
    constexpr double G_REAL = 6.67430e-11;            // gravitational constant (m³/kg·s²)
    constexpr double C_REAL = 299792458.0;            // speed of light (m/s)
    constexpr double SOLAR_MASS = 1.98892e30;         // solar mass (kg)

    // some default black holes
    constexpr double SAGITTARIUS_A_STAR_MASS = 4.31e6 * SOLAR_MASS / 5.0f;  // Milky Way's supermassive black hole
    constexpr double M87_STAR_MASS = 6.5e9 * SOLAR_MASS;             // M87 galaxy's supermassive black hole

    // ==================== Simulation Space Definition ====================
    // this is the total distance our sim world radius represents (in m)
    constexpr double REAL_WORLD_RADIUS_METERS = 1.0e11;

    // this is the size of our simulation world in "sim units"
    constexpr float SIM_WORLD_RADIUS = REAL_WORLD_RADIUS_METERS / C_REAL;

    // ==================== Default Simulation Parameters ====================
    // black hole mass to generate usually, we just use the saggitatius one as default rn (Milky Ways supermassive black hole)
    constexpr double DEFAULT_BLACK_HOLE_MASS = SAGITTARIUS_A_STAR_MASS;

    // here we define the accretion disk parameters, in Rs multiples.
    constexpr float DISK_INNER_RADIUS_RS = 3.0f;
    constexpr float DISK_OUTER_RADIUS_RS = 9.0f;
    constexpr float DISK_HEIGHT = 0.1f;
    const std::string DISK_NOISE_PATH = TEXTURE_PATH + "perlinNoise.png";

    constexpr glm::vec4 DISK_COLOR_HOT = glm::vec4(1.0f, 0.8f, 0.3f, 1.0f);
    constexpr glm::vec4 DISK_COLOR_COOL = glm::vec4(1.0f, 0.2f, 0.0f, 1.0f);

    // the default render mode of the accretion disk. {0: flat plane, 1: incline-based volume, 2: raymarched volume}
    constexpr unsigned int DISK_RENDER_MODE = 2;

    constexpr float DISK_ABSORPTION_COEFFICIENT = 5.0f;
    constexpr unsigned int DISK_MAX_MARCH_STEPS = 32;
    constexpr float DISK_MARCH_STEP_SIZE = 0.5f; // su
    constexpr bool DISK_USE_NOISE = true;

    // radial mesh params
    constexpr int DEFAULT_RADIAL_MESH_RINGS = 30;
    constexpr int DEFAULT_RADIAL_MESH_SPOKES = 36;
    constexpr int RADIAL_MESH_INNER_MARGIN = 1;    // in su
    constexpr int RADIAL_MESH_OUTER_MARGIN = 10;    // in su
    constexpr bool DEFAULT_RADIAL_MESH_ENABLE = true;
    constexpr glm::vec4 DEFAULT_RADIAL_MESH_COLOR = glm::vec4(1.0f, 1.0f, 1.0f, 0.5f);

    // the amount of steps the rays for the raytracer will take
    constexpr int DEFAULT_MAX_RAY_STEPS = 10000;
    constexpr float DEFAULT_RAY_STEP_SIZE = 1.0f; // in su (so every step is the amount of lightseconds)

    const std::string PLANETS_TXT_FILE = PLANETS_PATH + "base_planets.txt";
    const int PLANET_TEXTURE_WIDTH = 512;   // px
    const int PLANET_TEXTURE_HEIGHT = 256;  // px
    const int MAX_TEXTURE_COUNT = 16;
    constexpr float PLANET_AMBIENT_LIGHT = 0.06f;

    // ==================== Rendering ====================
    constexpr int SCREEN_WIDTH = 800 * 2;
    constexpr int SCREEN_HEIGHT = 600 * 2;
    // decides in what resolution we show the simulation (so we can upscale? Like, if its 2 then 4 pixels use 1 ray)
    constexpr float RENDER_SCALE = 0.5f;

    // workgroup sizes for the raytracer compute shader
    constexpr int WORKGROUP_SIZE_X = 8;
    constexpr int WORKGROUP_SIZE_Y = 8;

    // shader paths
    const std::string SCREENQUAD_VERT = SHADER_PATH + "screenQuad.vert";
    const std::string SCREENQUAD_FRAG = SHADER_PATH + "screenQuad.frag";
    const std::string RADIALMESH_VERT = SHADER_PATH + "radialMesh.vert";
    const std::string RADIALMESH_FRAG = SHADER_PATH + "radialMesh.frag";
    const std::string COMPUTE_RAYTRACE = COMPUTE_PATH + "simulation.comp";

    const std::string COMPUTE_PINN = COMPUTE_PATH + "pinn.comp";

    // ====================== PINN =======================
    constexpr unsigned int SCAN_ROW_BATCH_SIZE = 10;
}

#endif //BLACK_HOLE_SIMULATION_CONFIG_H
