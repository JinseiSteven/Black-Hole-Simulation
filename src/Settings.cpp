//
// Created by StephanVisser on 11/13/2025.
//

#include "../include/Settings.h"

#include "Config.h"

Settings::Settings() :
    m_physics_dirty(false),
    m_render_dirty(false)
{
    // we build the meters to sim units conversion (this way we can calculate with floats instead of doubles)
    m_scale.meters_per_sim_unit = Config::REAL_WORLD_RADIUS_METERS / Config::SIM_WORLD_RADIUS;

    // default config for the black hole and light params
    m_simulation.black_hole_mass_kg = Config::DEFAULT_BLACK_HOLE_MASS;
    m_simulation.max_ray_steps = Config::DEFAULT_MAX_RAY_STEPS;
    m_simulation.ray_step_size = Config::DEFAULT_RAY_STEP_SIZE;

    // also initializing some rendering settings up front
    m_render.render_scale = Config::RENDER_SCALE;
    m_render.radial_mesh_rings = Config::DEFAULT_RADIAL_MESH_RINGS;
    m_render.radial_mesh_spokes = Config::DEFAULT_RADIAL_MESH_SPOKES;
    m_render.enable_radial_mesh = Config::DEFAULT_RADIAL_MESH_ENABLE;
    m_render.radial_mesh_opacity = Config::BASE_RADIAL_MESH_OPACITY;

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
        m_physics_dirty = true;
    }
}

// renderer
void Settings::SetRenderScale(const float scale) {
    if (m_render.render_scale != scale) {
        m_render.render_scale = scale;
        m_render_dirty = true;
    }
}

void Settings::SetRadialMeshResolution(const int rings, const int spokes) {
    if (m_render.radial_mesh_rings != rings || m_render.radial_mesh_spokes != spokes) {
        m_render.radial_mesh_rings = rings;
        m_render.radial_mesh_spokes = spokes;
        m_render_dirty = true;
    }
}

void Settings::SetRadialMeshEnabled(const bool enabled) {
    if (m_render.enable_radial_mesh != enabled) {
        m_render.enable_radial_mesh = enabled;
        m_render_dirty = true;
    }
}

void Settings::SetRadialMeshOpacity(const float opacity) {
    if (m_render.radial_mesh_opacity != opacity) {
        m_render.radial_mesh_opacity = opacity;
        m_render_dirty = true;
    }
}

// simulation
void Settings::SetMaxRaySteps(const int steps) {
    if (m_simulation.max_ray_steps != steps) {
        m_simulation.max_ray_steps = steps;
        m_physics_dirty = true;
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

// change consumers
bool Settings::ConsumePhysicsChanges() const {  // TODO, check whether actually necessary???
    bool changed = m_physics_dirty;
    m_physics_dirty = false;
    return changed;
}

bool Settings::ConsumeRenderChanges() const {
    bool changed = m_render_dirty;
    m_render_dirty = false;
    return changed;
}

void Settings::RecalculateRs() {
    // first we calculate the Rs with very high precision, using our normal constants
    m_scale.real_rs_meters = (2.0 * Config::G_REAL * m_simulation.black_hole_mass_kg) /
                                 (Config::C_REAL * Config::C_REAL);

    // then, we scale it to our decided simulation units to remove need of doubles during further calculations
    m_scale.sim_rs = static_cast<float>(m_scale.real_rs_meters / m_scale.meters_per_sim_unit);
}
