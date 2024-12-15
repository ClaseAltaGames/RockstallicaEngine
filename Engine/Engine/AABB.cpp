#include "AABB.h"

bool AABB::Intersects(const AABB& other) const {
    return (minPoint.x <= other.maxPoint.x && maxPoint.x >= other.minPoint.x) &&
        (minPoint.y <= other.maxPoint.y && maxPoint.y >= other.minPoint.y) &&
        (minPoint.z <= other.maxPoint.z && maxPoint.z >= other.minPoint.z);
}

void AABB::Update(const glm::vec3& point) {
    minPoint.x = std::min(minPoint.x, point.x);
    minPoint.y = std::min(minPoint.y, point.y);
    minPoint.z = std::min(minPoint.z, point.z);
    maxPoint.x = std::max(maxPoint.x, point.x);
    maxPoint.y = std::max(maxPoint.y, point.y);
    maxPoint.z = std::max(maxPoint.z, point.z);
}

//bool RayIntersectsAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const AABB& box, float& t) {
//    float tmin = (box.GetMin().x - rayOrigin.x) / rayDir.x;
//    float tmax = (box.GetMax().x - rayOrigin.x) / rayDir.x;
//
//    if (tmin > tmax) std::swap(tmin, tmax);
//
//    float tymin = (box.GetMin().y - rayOrigin.y) / rayDir.y;
//    float tymax = (box.GetMax().y - rayOrigin.y) / rayDir.y;
//
//    if (tymin > tymax) std::swap(tymin, tymax);
//
//    if ((tmin > tymax) || (tymin > tmax))
//        return false;
//
//    if (tymin > tmin)
//        tmin = tymin;
//
//    if (tymax < tmax)
//        tmax = tymax;
//
//    float tzmin = (box.GetMin().z - rayOrigin.z) / rayDir.z;
//    float tzmax = (box.GetMax().z - rayOrigin.z) / rayDir.z;
//
//    if (tzmin > tzmax) std::swap(tzmin, tzmax);
//
//    if ((tmin > tzmax) || (tzmin > tmax))
//        return false;
//
//    if (tzmin > tmin)
//        tmin = tzmin;
//
//    if (tzmax < tmax)
//        tmax = tzmax;
//
//    t = tmin;
//
//    return true;
//}