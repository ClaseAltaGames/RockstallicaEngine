#pragma once

#include <algorithm>
#include "glm/glm.hpp"
#include "iostream"

class AABB {
public:
    AABB(const glm::vec3& minPoint, const glm::vec3& maxPoint)
        : minPoint(minPoint), maxPoint(maxPoint) {}

    bool Intersects(const AABB& other) const;
    void Update(const glm::vec3& point);
    glm::vec3 GetMin() const { return minPoint; }
    glm::vec3 GetMax() const { return maxPoint; }

private:
    glm::vec3 minPoint;
    glm::vec3 maxPoint;
};

//extern bool RayIntersectsAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const AABB& aabb, float& t);