//
// Created by StephanVisser on 11/13/2025.
//

#ifndef BLACK_HOLE_SIMULATION_SETTINGS_H
#define BLACK_HOLE_SIMULATION_SETTINGS_H

#include <glm/vec4.hpp>


class Settings {
public:
    Settings();
    ~Settings() = default;

    // renderer
    void SetRenderScale(float scale);
    void SetRadialMeshResolution(int rings, int spokes);
    void SetRadialMeshEnabled(bool enabled);
    void SetRadialMeshOpacity(float opacity);

    // sim
    void SetBlackHoleMass(double mass_kg);
    void SetMaxRaySteps(int steps);
    void SetRayStepSize(float step_size);

    // disk
    void SetDiskRadii(float inner_radius, float outer_radius);
    void SetDiskHeight(float min_height, float max_height);
    void SetDiskRenderMode(unsigned int mode);
    void SetDiskAbsorptionCoefficient(float coefficient);
    void SetDiskMarchSettings(unsigned int max_march_steps, float march_step_size);
    void SetDiskColors(glm::vec4 color_cool, glm::vec4 color_hot);

    // conversion
    float RealToSim(double real_meters) const;
    double SimToReal(float sim_units) const;

    double GetRealRsMeters() const;
    double GetMetersPerSimUnit() const;
    double GetBlackHoleMass() const;

    float GetDiskInnerRadius() const { return m_disk.inner_radius * GetSimRs(); }
    float GetDiskOuterRadius() const { return m_disk.outer_radius * GetSimRs(); }
    float GetDiskMinHeight() const { return m_disk.min_height * GetSimRs(); }
    float GetDiskMaxHeight() const { return m_disk.max_height * GetSimRs(); }
    unsigned int GetDiskRenderMode() const { return m_disk.renderMode; }
    float GetDiskAbsorptionCoeff() const { return m_disk.absorptionCoeff; }
    unsigned int GetDiskMaxMarchSteps() const { return m_disk.maxMarchSteps; }
    float GetDiskMarchStepSize() const { return m_disk.marchStepSize; }
    glm::vec4 GetDiskColorCool() const { return m_disk.colorCool; }
    glm::vec4 GetDiskColorHot() const { return m_disk.colorHot; }
    float GetDiskInnerRadiusRatio() const { return m_disk.inner_radius; }
    float GetDiskOuterRadiusRatio() const { return m_disk.outer_radius; }
    float GetDiskMinHeightRatio() const { return m_disk.min_height; }
    float GetDiskMaxHeightRatio() const { return m_disk.max_height; }

    float GetRenderScale() const { return m_render.render_scale; }
    int GetRadialMeshRings() const { return m_render.radial_mesh_rings; }
    int GetRadialMeshSpokes() const { return m_render.radial_mesh_spokes; }
    bool IsRadialMeshEnabled() const { return m_render.enable_radial_mesh; }
    float GetRadialMeshOpacity() const { return m_render.radial_mesh_opacity; }

    int GetMaxRaySteps() const { return m_simulation.max_ray_steps; }
    float GetRayStepSize() const { return m_simulation.ray_step_size; }
    float GetSimRs() const;
    float GetSimWorldRadius() const;
    float GetRadialMeshInnerRadius() const;
    float GetRadialMeshOuterRadius() const;

    float GetOrbitSensitivity() const { return m_camera.orbit_sensitivity; }
    float GetZoomSensitivity() const { return m_camera.zoom_sensitivity; }

    unsigned int GetSimulationVersion() const { return m_simulation_version; }
    unsigned int GetDiskVersion() const { return m_disk_version; }

    bool ConsumeRenderChanges() const;

private:

    unsigned int m_simulation_version{0};
    unsigned int m_disk_version{0};

    void RecalculateRs();

    struct ScaleData {
        double meters_per_sim_unit;
        double real_rs_meters;
        float sim_rs;
    } m_scale{};

    struct SimulationParams {
        double black_hole_mass_kg;
        int max_ray_steps;
        float ray_step_size;
    } m_simulation{};

    struct AccretionDiskParams {
        float inner_radius;
        float outer_radius;
        float min_height;
        float max_height;
        unsigned int renderMode;
        float absorptionCoeff;
        unsigned int maxMarchSteps;
        float marchStepSize;
        glm::vec4 colorHot;
        glm::vec4 colorCool;
    } m_disk{};

    struct RenderParams {
        float render_scale;
        int radial_mesh_rings;
        int radial_mesh_spokes;
        bool enable_radial_mesh;
        float radial_mesh_opacity;
    } m_render{};

    struct CameraParams {
        float orbit_sensitivity;
        float zoom_sensitivity;
    } m_camera{};

    mutable bool m_render_dirty;
};


#endif //BLACK_HOLE_SIMULATION_SETTINGS_H