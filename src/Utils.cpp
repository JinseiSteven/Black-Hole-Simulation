//
// Created by StephanVisser on 11/14/2025.
//

#include <cmath>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Utils.h"


namespace Utils {

    glm::vec3 cartesianToSpherical(const glm::vec3& pos) {
        const float r = glm::length(pos);
        if (r < 0.0001f) {
            return {0.0f, 0.0f, 0.0f};
        }

        // inclination -> 0 at +Y pole to pi at -Y pole
        const float theta = std::acos(pos.y / r);

        // azimuth -> -pi to +pi
        const float phi = std::atan2(pos.z, pos.x);

        return {r, theta, phi};
    }


    glm::vec3 sphericalToCartesian(const glm::vec3& spherical) {
        const float r = spherical.x;
        const float theta = spherical.y;
        const float phi = spherical.z;

        const float x = r * std::sin(theta) * std::cos(phi);
        const float y = r * std::cos(theta);
        const float z = r * std::sin(theta) * std::sin(phi);

        return {x, y, z};
    }

    std::vector<glm::vec4> LoadPlanetsFromFile(const std::string& path) {
        std::vector<glm::vec4> planets;
        std::ifstream file(path);

        if (!file.is_open()) {
            std::cerr << "Failed to open planets file: " << path << std::endl;
            return planets;
        }

        std::string line;
        while (std::getline(file, line)) {
            // skipping empty lines and comment lines
            if (line.empty() || line[0] == '#') continue;

            std::stringstream ss(line);
            float x, y, z, radius;
            char comma;

            ss >> x >> comma >> y >> comma >> z >> comma >> radius;

            planets.emplace_back(x, y, z, radius);
        }

        std::cout << "Loaded " << planets.size() << " planets from " << path << std::endl;
        return planets;
    }

}