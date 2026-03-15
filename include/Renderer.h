//
// Created by StephanVisser on 10/26/2025.
//

#ifndef BLACK_HOLE_SIMULATION_RENDERER_H
#define BLACK_HOLE_SIMULATION_RENDERER_H
#include <memory>
#include <glm/vec3.hpp>

class Camera;
class Shader;
class Simulation;
class Settings;


class Renderer {
public:
    Renderer(
        const Camera& camera,
        const Settings& settings,
        unsigned int texture_width,
        unsigned int texture_height,
        const char* quadVertexPath,
        const char* quadFragmentPath,
        const char* gridVertexPath,
        const char* gridFragmentPath);
    ~Renderer();

    void draw(bool skip_grid = false) const;
    void RebuildRadialMesh(int num_rings, int num_spokes, float inner_radius, float outer_radius);
    void UpdateRadialHeightmap(const std::vector<float>& radial_height_map) const;

    [[nodiscard]] unsigned int GetScreenTextureID() const;
private:
    const Camera& m_camera;
    const Settings& m_settings;
    std::unique_ptr<Shader> m_radial_shader;
    int m_current_rings;

    // our GP buffers for the perspective radial
    unsigned int m_radial_VAO{0}, m_radial_VBO{0}, m_radial_EBO{0};
    int m_gridIndexCount{0};
    bool m_grid_enabled{true};

    std::unique_ptr<Shader> m_quad_shader;

    // our GP buffers for the screen quad we will display
    unsigned int m_quad_VAO{0}, m_quad_VBO{0}, m_quad_EBO{0};

    unsigned int m_screen_texture{0}, m_radial_map_texture{0};
    unsigned int m_output_texture_width, m_output_texture_height;

    static void GenerateRadialMeshData(
        int num_rings, int num_spokes, float inner_radius, float outer_radius,
        std::vector<glm::vec3>& out_vertices,
        std::vector<unsigned int>& out_indices
    );

    void UploadRadialMeshData(
        const std::vector<glm::vec3>& vertices,
        const std::vector<unsigned int>& indices
    );

    void DestroyRadialMeshBuffers();

    void CreateQuadVAO();
    void CreateScreenTexture();
    void CreateRadialMapTexture(int rings);
};


#endif //BLACK_HOLE_SIMULATION_RENDERER_H