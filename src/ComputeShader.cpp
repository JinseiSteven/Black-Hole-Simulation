//
// Created by StephanVisser on 10/26/2025.
//

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

#include "../include/ComputeShader.h"

ComputeShader::ComputeShader(const char* computePath) {
    std::string computeCode;
    std::ifstream cShaderFile;

    cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        cShaderFile.open(computePath);
        std::stringstream cShaderStream;
        cShaderStream << cShaderFile.rdbuf();
        cShaderFile.close();
        computeCode = cShaderStream.str();
    }
    catch (std::ifstream::failure e) {
        std::cout << "ERROR::COMPUTESHADER::FILE_ERROR" << std::endl;
    }
    const char* cShaderCode = computeCode.c_str();

    unsigned int compute;
    int success;
    char infoLog[512];

    compute = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute, 1, &cShaderCode, nullptr);
    glCompileShader(compute);
    glGetShaderiv(compute, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(compute, 512, nullptr, infoLog);
        std::cout << "ERROR::COMPUTESHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    ID = glCreateProgram();
    glAttachShader(ID, compute);
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        std::cout << "ERROR::COMPUTESHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(compute);
}

ComputeShader::~ComputeShader() {
    glDeleteProgram(ID);
}

void ComputeShader::use() const {
    glUseProgram(ID);
}

void ComputeShader::SetBool(const std::string &name, const bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(value));
}

void ComputeShader::SetInt(const std::string &name, const int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void ComputeShader::SetFloat(const std::string &name, const float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void ComputeShader::SetMat4(const std::string& name, const glm::mat4& value) const {
    GLint location = glGetUniformLocation(ID, name.c_str());
    glUniformMatrix4fv(
        location,    // ID of the uniform
        1,           // amount of matrices?
        GL_FALSE,    // transpose? yes or no
        glm::value_ptr(value) // pointer to the matrix data (since it's a "v" type uniform setter)
    );
}

void ComputeShader::SetMat3(const std::string& name, const glm::mat3& value) const {
    const GLint location = glGetUniformLocation(ID, name.c_str());
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void ComputeShader::SetVec4(const std::string& name, const glm::vec4& value) const {
    const GLint location = glGetUniformLocation(ID, name.c_str());
    glUniform4fv(location, 1, glm::value_ptr(value));
}

void ComputeShader::SetVec3(const std::string& name, const glm::vec3& value) const {
    const GLint location = glGetUniformLocation(ID, name.c_str());
    glUniform3fv(location, 1, glm::value_ptr(value));
}

void ComputeShader::SetVec2(const std::string& name, const glm::vec2& value) const {
    const GLint location = glGetUniformLocation(ID, name.c_str());
    glUniform2fv(location, 1, glm::value_ptr(value));
}
