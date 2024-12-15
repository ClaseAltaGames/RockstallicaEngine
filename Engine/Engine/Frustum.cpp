#include "AABB.h"
#include "glm/glm.hpp"

struct Plane {
    glm::vec3 normal;
    float distance;

    float DistanceToPoint(const glm::vec3& point) const {
        return glm::dot(normal, point) + distance;
    }
};

struct Frustum {
    Plane planes[6];

    bool IsPointInside(const glm::vec3& point) const {
        for (int i = 0; i < 6; ++i) {
            if (planes[i].DistanceToPoint(point) < 0) {
                return false;
            }
        }
        return true;
    }

    bool IsAABBInside(const AABB& box) const {
        for (int i = 0; i < 6; ++i) {
            glm::vec3 positiveVertex = box.max;
            glm::vec3 negativeVertex = box.min;

            // Ajusta cada componente dependiendo del lado del plano
            if (planes[i].normal.x >= 0) {
                negativeVertex.x = box.min.x;
                positiveVertex.x = box.max.x;
            }
            else {
                negativeVertex.x = box.max.x;
                positiveVertex.x = box.min.x;
            }

            if (planes[i].normal.y >= 0) {
                negativeVertex.y = box.min.y;
                positiveVertex.y = box.max.y;
            }
            else {
                negativeVertex.y = box.max.y;
                positiveVertex.y = box.min.y;
            }

            if (planes[i].normal.z >= 0) {
                negativeVertex.z = box.min.z;
                positiveVertex.z = box.max.z;
            }
            else {
                negativeVertex.z = box.max.z;
                positiveVertex.z = box.min.z;
            }

            if (planes[i].DistanceToPoint(positiveVertex) < 0) {
                return false;
            }
        }
        return true;
    }
};


void ExtractFrustumPlanes(Frustum& frustum, const glm::mat4& viewProjectionMatrix) {
    glm::mat4 matrix = glm::transpose(viewProjectionMatrix);

    // Plano izquierdo
    frustum.planes[0].normal = glm::vec3(matrix[3] + matrix[0]);
    frustum.planes[0].distance = matrix[3][3] + matrix[0][3];

    // Plano derecho
    frustum.planes[1].normal = glm::vec3(matrix[3] - matrix[0]);
    frustum.planes[1].distance = matrix[3][3] - matrix[0][3];

    // Plano inferior
    frustum.planes[2].normal = glm::vec3(matrix[3] + matrix[1]);
    frustum.planes[2].distance = matrix[3][3] + matrix[1][3];

    // Plano superior
    frustum.planes[3].normal = glm::vec3(matrix[3] - matrix[1]);
    frustum.planes[3].distance = matrix[3][3] - matrix[1][3];

    // Plano cercano
    frustum.planes[4].normal = glm::vec3(matrix[3] + matrix[2]);
    frustum.planes[4].distance = matrix[3][3] + matrix[2][3];

    // Plano lejano
    frustum.planes[5].normal = glm::vec3(matrix[3] - matrix[2]);
    frustum.planes[5].distance = matrix[3][3] - matrix[2][3];

    // Normalizar los planos
    for (int i = 0; i < 6; i++) {
        float length = glm::length(frustum.planes[i].normal);
        frustum.planes[i].normal /= length;
        frustum.planes[i].distance /= length;
    }
}