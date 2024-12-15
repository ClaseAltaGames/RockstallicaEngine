#include "RayIntersectsAABB.h"

bool RayIntersectsAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const AABB& aabb, float& t) {
    glm::vec3 aabbMin = aabb.GetMin();
    glm::vec3 aabbMax = aabb.GetMax();

    float tmin = (aabbMin.x - rayOrigin.x) / rayDirection.x;
    float tmax = (aabbMax.x - rayOrigin.x) / rayDirection.x;

    if (tmin > tmax) std::swap(tmin, tmax);

    float tymin = (aabbMin.y - rayOrigin.y) / rayDirection.y;
    float tymax = (aabbMax.y - rayOrigin.y) / rayDirection.y;

    if (tymin > tymax) std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax))
        return false;

    if (tymin > tmin)
        tmin = tymin;

    if (tymax < tmax)
        tmax = tymax;

    float tzmin = (aabbMin.z - rayOrigin.z) / rayDirection.z;
    float tzmax = (aabbMax.z - rayOrigin.z) / rayDirection.z;

    if (tzmin > tzmax) std::swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;

    if (tzmin > tmin)
        tmin = tzmin;

    if (tzmax < tmax)
        tmax = tzmax;

    t = tmin;
    return true;
}