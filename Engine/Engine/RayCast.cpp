#include "RayCast.h"
#include <utility> 

RayCast::RayCast()
{
}

RayCast::~RayCast()
{
}

bool RayCast::IntersectsAABB(const glm::vec3& origin, const glm::vec3& direction, const glm::vec3& min, const glm::vec3& max) const
{
	float tmin = (min.x - origin.x) / direction.x;
	float tmax = (max.x - origin.x) / direction.x;

	if (tmin > tmax) std::swap(tmin, tmax);

	float tymin = (min.y - origin.y) / direction.y;
	float tymax = (max.y - origin.y) / direction.y;

	if (tymin > tymax) std::swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax))
		return false;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	float tzmin = (min.z - origin.z) / direction.z;
	float tzmax = (max.z - origin.z) / direction.z;

	if (tzmin > tzmax) std::swap(tzmin, tzmax);

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;

	return true;
}

bool RayCast::IntersectsPlane(const glm::vec3& origin, const glm::vec3& direction, const glm::vec3& point, const glm::vec3& normal) const
{
	float denom = glm::dot(normal, direction);
	if (denom > 1e-6)
	{
		glm::vec3 p0l0 = point - origin;
		float t = glm::dot(p0l0, normal) / denom;
		return t >= 0;
	}
	return false;
}

bool RayCast::IntersectsGameObject(const glm::vec3& origin, const glm::vec3& direction, const glm::vec3& min, const glm::vec3& max) const
{
	return IntersectsAABB(origin, direction, min, max);
}
