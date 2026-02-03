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
    void SetRadialMeshColor(const glm::vec4& color);

    // sim
    void SetBlackHoleMass(double mass_kg);
    void SetMaxRaySteps(int steps);
    void SetRayStepSize(float step_size);
    void SetPlanetAmbientLight(float ambient);

    // disk
    void SetDiskRadii(float inner_radius, float outer_radius);
    void SetDiskHeight(float height);
    void SetDiskRenderMode(unsigned int mode);
    void SetDiskAbsorptionCoefficient(float coefficient);
    void SetDiskMarchSettings(unsigned int max_march_steps, float march_step_size);
    void SetDiskColors(glm::vec4 color_cool, glm::vec4 color_hot);
    void SetDiskUseNoise(bool use_noise);

    void SetOrbitSensitivity(float sensitivity);
    void SetZoomSensitivity(float sensitivity);

    // conversion
    [[nodiscard]] float RealToSim(double real_meters) const;
    [[nodiscard]] double SimToReal(float sim_units) const;

    [[nodiscard]] double GetRealRsMeters() const;
    [[nodiscard]] double GetMetersPerSimUnit() const;
    [[nodiscard]] double GetBlackHoleMass() const;

    [[nodiscard]] float GetDiskInnerRadius() const { return m_disk.inner_radius * GetSimRs(); }
    [[nodiscard]] float GetDiskOuterRadius() const { return m_disk.outer_radius * GetSimRs(); }
    [[nodiscard]] float GetDiskHeight() const { return m_disk.height * GetSimRs(); }
    [[nodiscard]] unsigned int GetDiskRenderMode() const { return m_disk.renderMode; }
    [[nodiscard]] float GetDiskAbsorptionCoeff() const { return m_disk.absorptionCoeff; }
    [[nodiscard]] unsigned int GetDiskMaxMarchSteps() const { return m_disk.maxMarchSteps; }
    [[nodiscard]] float GetDiskMarchStepSize() const { return m_disk.marchStepSize; }
    [[nodiscard]] glm::vec4 GetDiskColorCool() const { return m_disk.colorCool; }
    [[nodiscard]] glm::vec4 GetDiskColorHot() const { return m_disk.colorHot; }
    [[nodiscard]] float GetDiskInnerRadiusRatio() const { return m_disk.inner_radius; }
    [[nodiscard]] float GetDiskOuterRadiusRatio() const { return m_disk.outer_radius; }
    [[nodiscard]] float GetDiskHeightRatio() const { return m_disk.height; }
    [[nodiscard]] bool GetDiskUseNoise() const { return m_disk.useNoise; }

    [[nodiscard]] float GetRenderScale() const { return m_render.render_scale; }
    [[nodiscard]] int GetRadialMeshRings() const { return m_render.radial_mesh_rings; }
    [[nodiscard]] int GetRadialMeshSpokes() const { return m_render.radial_mesh_spokes; }
    [[nodiscard]] bool IsRadialMeshEnabled() const { return m_render.enable_radial_mesh; }
    [[nodiscard]] glm::vec4 GetRadialMeshColor() const { return m_render.radial_mesh_color; }

    [[nodiscard]] int GetMaxRaySteps() const { return m_simulation.max_ray_steps; }
    [[nodiscard]] float GetRayStepSize() const { return m_simulation.ray_step_size; }
    [[nodiscard]] float GetPlanetAmbientLight() const { return m_simulation.planet_ambient_light; }
    [[nodiscard]] float GetSimRs() const;
    [[nodiscard]] float GetSimWorldRadius() const;
    [[nodiscard]] float GetRadialMeshInnerRadius() const;
    [[nodiscard]] float GetRadialMeshOuterRadius() const;

    [[nodiscard]] float GetOrbitSensitivity() const { return m_camera.orbit_sensitivity; }
    [[nodiscard]] float GetZoomSensitivity() const { return m_camera.zoom_sensitivity; }

    [[nodiscard]] unsigned int GetSimulationVersion() const { return m_simulation_version; }
    [[nodiscard]] unsigned int GetDiskVersion() const { return m_disk_version; }
    [[nodiscard]] unsigned int GetRenderVersion() const { return m_render_version; }
    [[nodiscard]] unsigned int GetCameraVersion() const { return m_camera_version; }

private:

    unsigned int m_simulation_version{0};
    unsigned int m_disk_version{0};
    unsigned int m_render_version{0};
    unsigned int m_camera_version{0};

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
        float planet_ambient_light;
    } m_simulation{};

    struct AccretionDiskParams {
        float inner_radius;
        float outer_radius;
        float height;
        unsigned int renderMode;
        float absorptionCoeff;
        unsigned int maxMarchSteps;
        float marchStepSize;
        bool useNoise;
        glm::vec4 colorHot;
        glm::vec4 colorCool;
    } m_disk{};

    struct RenderParams {
        float render_scale;
        int radial_mesh_rings;
        int radial_mesh_spokes;
        bool enable_radial_mesh;
        glm::vec4 radial_mesh_color;
    } m_render{};

    struct CameraParams {
        float orbit_sensitivity;
        float zoom_sensitivity;
    } m_camera{};
};


#endif //BLACK_HOLE_SIMULATION_SETTINGS_H