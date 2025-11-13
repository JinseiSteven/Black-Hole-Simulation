//
// Created by StephanVisser on 11/13/2025.
//

#ifndef BLACK_HOLE_SIMULATION_SETTINGS_H
#define BLACK_HOLE_SIMULATION_SETTINGS_H


class Settings {
public:
    Settings();
    ~Settings() = default;

    // object setting(s)
    void SetBlackHoleMass(double mass_kg);

    // renderer
    void SetRenderScale(float scale);
    void SetRadialMeshResolution(int rings, int spokes);
    void SetRadialMeshEnabled(bool enabled);
    void SetRadialMeshOpacity(float opacity);

    // sim
    void SetMaxRaySteps(int steps);
    float GetSimRs() const;
    float GetSimWorldRadius() const;
    float GetRadialMeshInnerRadius() const;
    float GetRadialMeshOuterRadius() const;

    // conversion
    float RealToSim(double real_meters) const;
    double SimToReal(float sim_units) const;

    double GetRealRsMeters() const;
    double GetMetersPerSimUnit() const;
    double GetBlackHoleMass() const;

    float GetRenderScale() const { return m_render.render_scale; }
    int GetRadialMeshRings() const { return m_render.radial_mesh_rings; }
    int GetRadialMeshSpokes() const { return m_render.radial_mesh_spokes; }
    bool IsRadialMeshEnabled() const { return m_render.enable_radial_mesh; }
    float GetRadialMeshOpacity() const { return m_render.radial_mesh_opacity; }

    int GetMaxRaySteps() const { return m_simulation.max_ray_steps; }
    float GetRayStepSize() const { return m_simulation.ray_step_size; }

    float GetOrbitSensitivity() const { return m_camera.orbit_sensitivity; }
    float GetZoomSensitivity() const { return m_camera.zoom_sensitivity; }

    bool ConsumePhysicsChanges() const;
    bool ConsumeRenderChanges() const;

private:
    void RecalculateRs();

    struct ScaleData {
        double meters_per_sim_unit;
        double real_rs_meters;
        float sim_rs;
    } m_scale;

    struct SimulationParams {
        double black_hole_mass_kg;
        int max_ray_steps;
        float ray_step_size;
    } m_simulation;

    struct RenderParams {
        float render_scale;
        int radial_mesh_rings;
        int radial_mesh_spokes;
        bool enable_radial_mesh;
        float radial_mesh_opacity;
    } m_render;

    struct CameraParams {
        float orbit_sensitivity;
        float zoom_sensitivity;
    } m_camera;

    mutable bool m_physics_dirty;
    mutable bool m_render_dirty;
};


#endif //BLACK_HOLE_SIMULATION_SETTINGS_H