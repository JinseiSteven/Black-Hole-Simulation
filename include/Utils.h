//
// Created by StephanVisser on 11/14/2025.
//

#ifndef BLACK_HOLE_SIMULATION_UTILS_H
#define BLACK_HOLE_SIMULATION_UTILS_H

#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace Utils {
    glm::vec3 cartesianToSpherical(const glm::vec3& pos);
    glm::vec3 sphericalToCartesian(const glm::vec3& spherical);
    std::vector<glm::vec4> LoadPlanetsFromFile(const std::string& path);

}


#endif //BLACK_HOLE_SIMULATION_UTILS_H