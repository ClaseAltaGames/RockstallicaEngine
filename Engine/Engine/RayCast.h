#pragma once
#include "glm/glm.hpp"

class RayCast
{
public:
	RayCast();
	~RayCast();

	// Check if this ray intersects with an AABB
	bool IntersectsAABB(const glm::vec3& origin, const glm::vec3& direction, const glm::vec3& min, const glm::vec3& max) const;

	// Check if this ray intersects with a plane
	bool IntersectsPlane(const glm::vec3& origin, const glm::vec3& direction, const glm::vec3& point, const glm::vec3& normal) const;

	// Check if this ray intersects with a gameobject
	bool IntersectsGameObject(const glm::vec3& origin, const glm::vec3& direction, const glm::vec3& min, const glm::vec3& max) const;
};


