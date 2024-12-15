#pragma once
#include "glm/glm.hpp"
#include "AABB.h"

bool RayIntersectsAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const AABB& aabb, float& t);