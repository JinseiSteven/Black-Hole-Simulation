//
// Created by StephanVisser on 10/26/2025.
//

#ifndef BLACK_HOLE_SIMULATION_COMPUTESHADER_H
#define BLACK_HOLE_SIMULATION_COMPUTESHADER_H

#include <string>
#include <glm/glm.hpp>


class ComputeShader {
public:
    unsigned int ID;
    explicit ComputeShader(const char* computePath);
    ~ComputeShader();
    void use() const;
    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetMat4(const std::string& name, const glm::mat4& value) const;
    void SetMat3(const std::string& name, const glm::mat3& value) const;
    void SetVec4(const std::string& name, const glm::vec4& value) const;
    void SetVec3(const std::string& name, const glm::vec3& value) const;
    void SetVec2(const std::string& name, const glm::vec2& value) const;
};


#endif //BLACK_HOLE_SIMULATION_COMPUTESHADER_H