//
// Created by StephanVisser on 10/26/2025.
//

#include <GL/glew.h>
#include <cmath>
#include <glm/vec2.hpp>
#include <vector>

#include "Renderer.h"

#include <iostream>
#include <glm/common.hpp>
#include <glm/vec3.hpp>
#include <glm/ext/scalar_constants.hpp>

#include "Config.h"
#include "Shader.h"

Renderer::Renderer(
    const Camera* camera,
    const unsigned int texture_width,
    const unsigned int texture_height,
    const char* quadVertexPath,
    const char* quadFragmentPath,
    const char* gridVertexPath,
    const char* gridFragmentPath) :
    // creating the shader which will render our Quad
    m_camera(camera),
    m_radial_shader(std::make_unique<Shader>(gridVertexPath, gridFragmentPath)),
    m_quad_shader(std::make_unique<Shader>(quadVertexPath, quadFragmentPath)),
    m_output_texture_width(texture_width),
    m_output_texture_height(texture_height)
{
    // initializing the vertex buffers and the final output render texture
    CreateQuadVAO();
    CreateScreenTexture();
    CreateRadialMapTexture(Config::DEFAULT_RADIAL_MESH_RINGS);
    m_current_rings = Config::DEFAULT_RADIAL_MESH_RINGS;
}

Renderer::~Renderer() {
    // delete the buffers
    glDeleteVertexArrays(1, &m_quad_VAO);
    glDeleteBuffers(1, &m_quad_VBO);
    glDeleteBuffers(1, &m_quad_EBO);

    DestroyRadialMeshBuffers();

    glDeleteTextures(1, &m_screen_texture);
    glDeleteTextures(1, &m_radial_map_texture);
}


void Renderer::CreateQuadVAO() {
    // this is just the standard vertex data for a quad that is displayed over the entire screen
    constexpr float vertices[] = {
        // positions       // texture coords
        -1.0f,  -1.0f,     0.0f, 0.0f,      // bottom left
        -1.0f,   1.0f,     0.0f, 1.0f,      // top left
         1.0f,   1.0f,     1.0f, 1.0f,      // top right
         1.0f,  -1.0f,     1.0f, 0.0f       // bottom right
    };

    constexpr unsigned int indices[] = {
        0, 1, 3,        // bottom left triangle
        1, 2, 3         // top right triangle
    };

    glGenVertexArrays(1, &m_quad_VAO);
    glGenBuffers(1, &m_quad_VBO);
    glGenBuffers(1, &m_quad_EBO);

    glBindVertexArray(m_quad_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_quad_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quad_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Renderer::RebuildRadialMesh(const int num_rings, const int num_spokes, const float inner_radius, const float outer_radius) {
    if (num_rings != m_current_rings) {
        if (m_radial_map_texture != 0) {
            glDeleteTextures(1, &m_radial_map_texture);
            m_radial_map_texture = 0;
        }
        CreateRadialMapTexture(num_rings);
        m_current_rings = num_rings;
    }

    // destroy any existing radial mesh buffers we might have instantiated before
    DestroyRadialMeshBuffers();

    // construct our radial mesh
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;
    GenerateRadialMeshData(num_rings, num_spokes, inner_radius, outer_radius, vertices, indices);

    // upload the radial mesh to the OpenGL buffer
    UploadRadialMeshData(vertices, indices);

    // recording the amount of indices we have (since the glDraw function needs this)
    m_gridIndexCount = static_cast<int>(indices.size());
}

void Renderer::GenerateRadialMeshData(
    const int num_rings, const int num_spokes,
    const float inner_radius, const float outer_radius,
    std::vector<glm::vec3> &out_vertices, std::vector<unsigned int> &out_indices) {

    // first the vertex data will have the shape of:
    // [x, z, r]

    // NM vertices in total
    out_vertices.reserve(num_rings * num_spokes);
    constexpr float circle_radians = (2.0f * glm::pi<float>());

    // iterating over the rings
    for (int i = 0; i < num_rings; i++) {
        const float r_norm = (num_rings == 1) ? 0.0f : static_cast<float>(i) / static_cast<float>(num_rings - 1);

        // lerping between the min and max radius
        const float r_world = glm::mix(inner_radius, outer_radius, r_norm);

        // iterating over the spokes
        for (int j = 0; j < num_spokes; j++) {

            const float theta = (static_cast<float>(j) / static_cast<float>(num_spokes)) * circle_radians;

            float x = r_world * std::cos(theta);
            float z = r_world * std::sin(theta);

            out_vertices.emplace_back(x, z, r_norm);
        }
    }

    // 4NM - 2M indices in total for the EBO
    out_indices.reserve(4 * num_rings * num_spokes - 2 * num_spokes);

    // spokes (radial lines) (for all rings except for the outer, since there are no further rings)
    for (int i = 0; i < num_rings - 1; ++i) {
        for (int j = 0; j < num_spokes; ++j) {
            unsigned int innerRing = (i * num_spokes) + j;
            unsigned int outerRing = ((i + 1) * num_spokes) + j;
            out_indices.push_back(innerRing);
            out_indices.push_back(outerRing);
        }
    }
    // rings (tangential lines)
    for (int i = 0; i < num_rings; ++i) {
        for (int j = 0; j < num_spokes; ++j) {
            // has to be modulo M since we want to wrap around to the start as well
            unsigned int indexA = (i * num_spokes) + j;
            unsigned int indexB = (i * num_spokes) + ((j + 1) % num_spokes);
            out_indices.push_back(indexA);
            out_indices.push_back(indexB);
        }
    }
}

void Renderer::UploadRadialMeshData(const std::vector<glm::vec3> &vertices, const std::vector<unsigned int> &indices) {
    glGenVertexArrays(1, &m_radial_VAO);
    glGenBuffers(1, &m_radial_VBO);
    glGenBuffers(1, &m_radial_EBO);

    glBindVertexArray(m_radial_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_radial_VBO);
    glBufferData(GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3)),
        vertices.data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_radial_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)),
        indices.data(),
        GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Renderer::DestroyRadialMeshBuffers() {
    if (m_radial_VAO != 0) {
        glDeleteVertexArrays(1, &m_radial_VAO);
        m_radial_VAO = 0;
    }
    if (m_radial_VBO != 0) {
        glDeleteBuffers(1, &m_radial_VBO);
        m_radial_VBO = 0;
    }
    if (m_radial_EBO != 0) {
        glDeleteBuffers(1, &m_radial_EBO);
        m_radial_EBO = 0;
    }
    m_gridIndexCount = 0;
}


void Renderer::CreateScreenTexture() {
    glGenTextures(1, &m_screen_texture);
    glBindTexture(GL_TEXTURE_2D, m_screen_texture);

    // setting the texture wrapping params
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // setting the texture filtering params
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        static_cast<GLint>(m_output_texture_width),
        static_cast<GLint>(m_output_texture_height),
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr
    );

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::CreateRadialMapTexture(const int rings) {
    glGenTextures(1, &m_radial_map_texture);
    glBindTexture(GL_TEXTURE_1D, m_radial_map_texture);

    glTexImage1D(GL_TEXTURE_1D,
        0,
        GL_R32F,
        rings,
        0,
        GL_RED,
        GL_FLOAT,
        nullptr
    );

    // TODO, do I actually even need this?
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_1D, 0);
}


void Renderer::draw() const {
    // draw the quad and draw the texture given by the simulation on the quad
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // depth testing for correct rendering of the grid
    glEnable(GL_DEPTH_TEST);
    // enable blending as well, so we can render both the grid and our simulation
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ========================= Grid Rendering =========================
    // first we will draw the background 3d mesh for perspective

    m_radial_shader->use();

    m_radial_shader->SetMat4("VPM", m_camera->GetViewProjectionMatrix());
    m_radial_shader->SetVec4("gridColor", Config::DEFAULT_GRID_COLOR);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, m_radial_map_texture);

    m_radial_shader->SetInt("RadialMapTexture", 0);

    glBindVertexArray(m_radial_VAO);
    glDrawElements(GL_LINES, m_gridIndexCount, GL_UNSIGNED_INT, nullptr);

    // ========================= Screen Quad Drawing =========================
    glDisable(GL_DEPTH_TEST);

    m_quad_shader->use();

    // activating the first texture slot
    glActiveTexture(GL_TEXTURE0);

    // then, binding the screen_texture we chose to the currently active slot (so 0)
    glBindTexture(GL_TEXTURE_2D, m_screen_texture);

    // setting our uniform called "texture"
    m_quad_shader->SetInt("ScreenTexture", 0);

    glBindVertexArray(m_quad_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void Renderer::UpdateRadialHeightmap(const std::vector<float> &radial_height_map) const {
    glBindTexture(GL_TEXTURE_1D, m_radial_map_texture);
    glTexSubImage1D(GL_TEXTURE_1D,
        0,
        0,
        static_cast<GLsizei>(radial_height_map.size()),
        GL_RED,
        GL_FLOAT,
        radial_height_map.data()
    );
    glBindTexture(GL_TEXTURE_1D, 0);
}


unsigned int Renderer::GetScreenTextureID() const {
    return m_screen_texture;
}
