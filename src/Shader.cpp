//
// Created by StephanVisser on 10/26/2025.
//

// so we have all the OpenGL headers
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Shader.h"


Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // we have to turn on these flags in order to let the shaderfiles raise exceptions
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        // opening the files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);

        // defining streams so we can read the files buffer into them
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        // closing the files since we dont need them anymore
        vShaderFile.close();
        fShaderFile.close();

        // then, converting the streams into strings which we can then pass to the program
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e) {
        std::cout << "ERROR::SHADER::FILE_ERROR" << std::endl;
    }

    // now we have to convert them from strings to char* since this is what OpenGL accepts
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // creating the vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // creating the fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // lastly, we need to create our shader program by attaching our shaders and linking them
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // since both shaders are now compiled and linked inside of the program we created, we can safely delete them
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader() {
    glDeleteProgram(ID);
}

void Shader::use() const {
    glUseProgram(ID);
}

void Shader::SetBool(const std::string &name, const bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(value));
}

void Shader::SetInt(const std::string &name, const int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetFloat(const std::string &name, const float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value) const {
    GLint location = glGetUniformLocation(ID, name.c_str());
    glUniformMatrix4fv(
        location,    // ID of the uniform
        1,           // amount of matrices?
        GL_FALSE,    // transpose? yes or no
        glm::value_ptr(value) // pointer to the matrix data (since it's a "v" type uniform setter)
    );
}

void Shader::SetMat3(const std::string& name, const glm::mat3& value) const {
    const GLint location = glGetUniformLocation(ID, name.c_str());
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetVec4(const std::string& name, const glm::vec4& value) const {
    const GLint location = glGetUniformLocation(ID, name.c_str());
    glUniform4fv(location, 1, glm::value_ptr(value));
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) const {
    const GLint location = glGetUniformLocation(ID, name.c_str());
    glUniform3fv(location, 1, glm::value_ptr(value));
}

void Shader::SetVec2(const std::string& name, const glm::vec2& value) const {
    const GLint location = glGetUniformLocation(ID, name.c_str());
    glUniform2fv(location, 1, glm::value_ptr(value));
}

