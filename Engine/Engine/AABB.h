#pragma once

#include <algorithm>
#include "glm/glm.hpp"


class AABB {
public:
    glm::vec3 min; // Minimum point
    glm::vec3 max; // Maximum point

    // Constructor
    AABB(const glm::vec3& minPoint, const glm::vec3& maxPoint);

    // Check if this AABB intersects with another AABB
    bool Intersects(const AABB& other) const;

    // Update the AABB with a new point
    void Update(const glm::vec3& point);
};
