#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "AABB.h"

struct Plane {
    glm::vec3 normal;
    float distance;

    float DistanceToPoint(const glm::vec3& point) const;
};

class Frustum {
public:
    Frustum();
	~Frustum();

    Plane planes[6];
	glm::vec4 planesArray[6];

    bool IsPointInside(const glm::vec3& point) const;
    bool IsAABBInside(const AABB& box) const;
    void ExtractFrustumPlanes(Frustum& frustum, const glm::mat4& viewProjectionMatrix);
};
