#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "AABB.h" // Incluye tu clase AABB

struct Plane {
    glm::vec3 normal;
    float distance;

    float DistanceToPoint(const glm::vec3& point) const;
};

struct Frustum {
    Plane planes[6];

    bool IsPointInside(const glm::vec3& point) const;
    bool IsAABBInside(const AABB& box) const;
};

void ExtractFrustumPlanes(Frustum& frustum, const glm::mat4& viewProjectionMatrix);