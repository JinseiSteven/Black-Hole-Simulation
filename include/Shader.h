//
// Created by StephanVisser on 10/26/2025.
//

#ifndef BLACK_HOLE_SIMULATION_SHADER_H
#define BLACK_HOLE_SIMULATION_SHADER_H

#include <glm/glm.hpp>
#include <string>

// code for this shader class comes from the LearnOpenGL website: https://learnopengl.com/Getting-started/Shaders
class Shader {
public:
    // program ID of our shader
    unsigned int ID;

    // constructor in order to compile our shader from source files
    Shader(const char* vertexPath, const char* fragmentPath);

    ~Shader();

    // helper function to activate the shader
    void use() const;

    // more helper functions in order to set uniform variables in the shader
    void SetBool(const std::string &name, bool value) const;
    void SetInt(const std::string &name, int value) const;
    void SetFloat(const std::string &name, float value) const;
    void SetMat4(const std::string& name, const glm::mat4& value) const;
    void SetMat3(const std::string& name, const glm::mat3& value) const;
    void SetVec4(const std::string& name, const glm::vec4& value) const;
    void SetVec3(const std::string& name, const glm::vec3& value) const;
    void SetVec2(const std::string& name, const glm::vec2& value) const;
};


#endif //BLACK_HOLE_SIMULATION_SHADER_H