#include "AABB.h"

// Constructor
AABB::AABB(const  glm::vec3& minPoint, const  glm::vec3& maxPoint)
    : min(minPoint), max(maxPoint) {}

// Check if this AABB intersects with another AABB
bool AABB::Intersects(const AABB& other) const {
    return (min.x <= other.max.x && max.x >= other.min.x) &&
        (min.y <= other.max.y && max.y >= other.min.y) &&
        (min.z <= other.max.z && max.z >= other.min.z);
}

// Update the AABB with a new point
void AABB::Update(const  glm::vec3& point) {
    min.x = std::min(min.x, point.x);
    min.y = std::min(min.y, point.y);
    min.z = std::min(min.z, point.z);
    max.x = std::max(max.x, point.x);
    max.y = std::max(max.y, point.y);
    max.z = std::max(max.z, point.z);
}