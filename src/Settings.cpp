//
// Created by StephanVisser on 11/13/2025.
//

#include "Settings.h"

#include "Config.h"

Settings::Settings() {
    // we build the meters to sim units conversion (this way we can calculate with floats instead of doubles)
    m_scale.meters_per_sim_unit = Config::REAL_WORLD_RADIUS_METERS / Config::SIM_WORLD_RADIUS;

    // default config for the black hole and light params
    m_simulation.black_hole_mass_kg = Config::DEFAULT_BLACK_HOLE_MASS;
    m_simulation.max_ray_steps = Config::DEFAULT_MAX_RAY_STEPS;
    m_simulation.ray_step_size = Config::DEFAULT_RAY_STEP_SIZE;
    m_simulation.planet_ambient_light = Config::PLANET_AMBIENT_LIGHT;

    // setting all the default accretion disk params (hell of a lot of em)
    m_disk.inner_radius = Config::DISK_INNER_RADIUS_RS;
    m_disk.outer_radius = Config::DISK_OUTER_RADIUS_RS;
    m_disk.height = Config::DISK_HEIGHT;
    m_disk.renderMode = Config::DISK_RENDER_MODE;
    m_disk.absorptionCoeff = Config::DISK_ABSORPTION_COEFFICIENT;
    m_disk.maxMarchSteps = Config::DISK_MAX_MARCH_STEPS;
    m_disk.marchStepSize = Config::DISK_MARCH_STEP_SIZE;
    m_disk.useNoise = Config::DISK_USE_NOISE;
    m_disk.colorHot = Config::DISK_COLOR_HOT;
    m_disk.colorCool = Config::DISK_COLOR_COOL;

    // also initializing some rendering settings up front
    m_render.render_scale = Config::RENDER_SCALE;
    m_render.radial_mesh_rings = Config::DEFAULT_RADIAL_MESH_RINGS;
    m_render.radial_mesh_spokes = Config::DEFAULT_RADIAL_MESH_SPOKES;
    m_render.enable_radial_mesh = Config::DEFAULT_RADIAL_MESH_ENABLE;
    m_render.radial_mesh_color = Config::DEFAULT_RADIAL_MESH_COLOR;

    m_render.vsync = true;

    m_camera.orbit_sensitivity = Config::BASE_CAMERA_ORBIT_SENSITIVITY;
    m_camera.zoom_sensitivity = Config::BASE_CAMERA_ZOOM_SENSITIVITY;

    // calculating the initial physics based on these base values
    RecalculateRs();
}

// objects
void Settings::SetBlackHoleMass(const double mass_kg) {
    // check for whether the difference is great enough, since we might get weird floating point variance
    if (std::abs(m_simulation.black_hole_mass_kg - mass_kg) > 1e-10) {
        m_simulation.black_hole_mass_kg = mass_kg;
        RecalculateRs();
        m_simulation_version++;
    }
}

// renderer
void Settings::SetRenderScale(const float scale) {
    if (m_render.render_scale != scale) {
        m_render.render_scale = scale;
        m_render_version++;
    }
}

void Settings::SetRadialMeshResolution(const int rings, const int spokes) {
    if (m_render.radial_mesh_rings != rings || m_render.radial_mesh_spokes != spokes) {
        m_render.radial_mesh_rings = rings;
        m_render.radial_mesh_spokes = spokes;
        m_render_version++;
    }
}

void Settings::SetRadialMeshEnabled(const bool enabled) {
    if (m_render.enable_radial_mesh != enabled) {
        m_render.enable_radial_mesh = enabled;
        m_render_version++;
    }
}

void Settings::SetRadialMeshColor(const glm::vec4& color) {
    if (m_render.radial_mesh_color != color) {
        m_render.radial_mesh_color = color;
        m_render_version++;
    }
}


// simulation
void Settings::SetMaxRaySteps(const int steps) {
    if (m_simulation.max_ray_steps != steps) {
        m_simulation.max_ray_steps = steps;
        m_simulation_version++;
    }
}

void Settings::SetRayStepSize(const float step_size) {
    if (m_simulation.ray_step_size != step_size) {
        m_simulation.ray_step_size = step_size;
        m_simulation_version++;
    }
}

void Settings::SetPlanetAmbientLight(const float ambient) {
    if (m_simulation.planet_ambient_light != ambient) {
        m_simulation.planet_ambient_light = ambient;
        m_simulation_version++;
    }
}

void Settings::SetDiskRadii(const float inner_radius, const float outer_radius) {
    if (m_disk.inner_radius != inner_radius || m_disk.outer_radius != outer_radius) {
        m_disk.inner_radius = inner_radius;
        m_disk.outer_radius = outer_radius;
        m_disk_version++;
    }
}

void Settings::SetDiskHeight(const float height) {
    if (m_disk.height != height) {
        m_disk.height = height;
        m_disk_version++;
    }
}

void Settings::SetDiskRenderMode(const unsigned int mode) {
    if (m_disk.renderMode != mode) {
        m_disk.renderMode = mode;
        m_disk_version++;
    }
}

void Settings::SetDiskAbsorptionCoefficient(const float coefficient) {
    if (m_disk.absorptionCoeff != coefficient) {
        m_disk.absorptionCoeff = coefficient;
        m_disk_version++;
    }
}

void Settings::SetDiskMarchSettings(const unsigned int max_march_steps, const float march_step_size) {
    if (m_disk.maxMarchSteps != max_march_steps || m_disk.marchStepSize != march_step_size) {
        m_disk.maxMarchSteps = max_march_steps;
        m_disk.marchStepSize = march_step_size;
        m_disk_version++;
    }
}

void Settings::SetDiskColors(const glm::vec4 color_cool, const glm::vec4 color_hot) {
    if (m_disk.colorCool != color_cool || m_disk.colorHot != color_hot) {
        m_disk.colorCool = color_cool;
        m_disk.colorHot = color_hot;
        m_disk_version++;
    }
}

void Settings::SetDiskUseNoise(const bool use_noise) {
    if (m_disk.useNoise != use_noise) {
        m_disk.useNoise = use_noise;
        m_disk_version++;
    }
}

void Settings::SetVsync(const bool enabled) {
    if (m_render.vsync != enabled) {
        m_render.vsync = enabled;
        m_render_version++;
    }
}

void Settings::SetOrbitSensitivity(float sensitivity) {
    if (m_camera.orbit_sensitivity != sensitivity) {
        m_camera.orbit_sensitivity = sensitivity;
        m_camera_version++;
    }
}

void Settings::SetZoomSensitivity(float sensitivity) {
    if (m_camera.zoom_sensitivity != sensitivity) {
        m_camera.zoom_sensitivity = sensitivity;
        m_camera_version++;
    }
}

float Settings::GetSimRs() const {
    return m_scale.sim_rs;
}

float Settings::GetSimWorldRadius() const {
    return Config::SIM_WORLD_RADIUS;
}

float Settings::GetRadialMeshInnerRadius() const {
    return m_scale.sim_rs + Config::RADIAL_MESH_INNER_MARGIN;
}

float Settings::GetRadialMeshOuterRadius() const {
    return Config::SIM_WORLD_RADIUS - Config::RADIAL_MESH_OUTER_MARGIN;
}

// conversion
float Settings::RealToSim(const double real_meters) const {
    return static_cast<float>(real_meters / m_scale.meters_per_sim_unit);
}

double Settings::SimToReal(const float sim_units) const {
    return sim_units * m_scale.meters_per_sim_unit;
}

double Settings::GetRealRsMeters() const {
    return m_scale.real_rs_meters;
}

double Settings::GetMetersPerSimUnit() const {
    return m_scale.meters_per_sim_unit;
}

double Settings::GetBlackHoleMass() const {
    return m_simulation.black_hole_mass_kg;
}

void Settings::RecalculateRs() {
    // first we calculate the Rs with very high precision, using our normal constants
    m_scale.real_rs_meters = (2.0 * Config::G_REAL * m_simulation.black_hole_mass_kg) /
                                 (Config::C_REAL * Config::C_REAL);

    // then, we scale it to our decided simulation units to remove need of doubles during further calculations
    m_scale.sim_rs = static_cast<float>(m_scale.real_rs_meters / m_scale.meters_per_sim_unit);
}
