#include "AABB.h"
#include <iostream>

// Constructor por defecto
AABB::AABB() : min(0.0f), max(0.0f) {}

// Constructor con parámetros
AABB::AABB(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}

// Check if this AABB intersects with another AABB
bool AABB::Intersects(const AABB& other) const {
    return (min.x <= other.max.x && max.x >= other.min.x) &&
        (min.y <= other.max.y && max.y >= other.min.y) &&
        (min.z <= other.max.z && max.z >= other.min.z);
}

// Update the AABB with a new point
void AABB::Update(const glm::vec3& point) {
    min.x = std::min(min.x, point.x);
    min.y = std::min(min.y, point.y);
    min.z = std::min(min.z, point.z);
    max.x = std::max(max.x, point.x);
    max.y = std::max(max.y, point.y);
    max.z = std::max(max.z, point.z);
}

// Draw the bounding box
void AABB::DrawAABB(const glm::mat4& modelTransform) const {
	
    // Asegurarnos de que la AABB esté correctamente transformada
    AABB transformedAABB = GetAABB(modelTransform);

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glLineWidth(2.0f);  // Asegurarse de que la línea sea visible
    glColor3f(0.0f, 1.0f, 0.0f);  // Color verde para la caja

    // Dibujar las líneas de la caja
    glBegin(GL_LINES);

    // Front face
    glVertex3f(transformedAABB.min.x, transformedAABB.min.y, transformedAABB.min.z);
    glVertex3f(transformedAABB.max.x, transformedAABB.min.y, transformedAABB.min.z);
    glVertex3f(transformedAABB.max.x, transformedAABB.min.y, transformedAABB.min.z);
    glVertex3f(transformedAABB.max.x, transformedAABB.min.y, transformedAABB.max.z);
    glVertex3f(transformedAABB.max.x, transformedAABB.min.y, transformedAABB.max.z);
    glVertex3f(transformedAABB.min.x, transformedAABB.min.y, transformedAABB.max.z);
    glVertex3f(transformedAABB.min.x, transformedAABB.min.y, transformedAABB.max.z);
    glVertex3f(transformedAABB.min.x, transformedAABB.min.y, transformedAABB.min.z);

    // Bottom face
    glVertex3f(transformedAABB.min.x, transformedAABB.max.y, transformedAABB.min.z);
    glVertex3f(transformedAABB.max.x, transformedAABB.max.y, transformedAABB.min.z);
    glVertex3f(transformedAABB.max.x, transformedAABB.max.y, transformedAABB.min.z);
    glVertex3f(transformedAABB.max.x, transformedAABB.max.y, transformedAABB.max.z);
    glVertex3f(transformedAABB.max.x, transformedAABB.max.y, transformedAABB.max.z);
    glVertex3f(transformedAABB.min.x, transformedAABB.max.y, transformedAABB.max.z);
    glVertex3f(transformedAABB.min.x, transformedAABB.max.y, transformedAABB.max.z);
    glVertex3f(transformedAABB.min.x, transformedAABB.max.y, transformedAABB.min.z);

    // Left face
    glVertex3f(transformedAABB.min.x, transformedAABB.min.y, transformedAABB.min.z);
    glVertex3f(transformedAABB.min.x, transformedAABB.max.y, transformedAABB.min.z);
    glVertex3f(transformedAABB.min.x, transformedAABB.max.y, transformedAABB.min.z);
    glVertex3f(transformedAABB.min.x, transformedAABB.max.y, transformedAABB.max.z);
    glVertex3f(transformedAABB.min.x, transformedAABB.max.y, transformedAABB.max.z);
    glVertex3f(transformedAABB.min.x, transformedAABB.min.y, transformedAABB.max.z);

    // Right face
    glVertex3f(transformedAABB.max.x, transformedAABB.min.y, transformedAABB.min.z);
    glVertex3f(transformedAABB.max.x, transformedAABB.max.y, transformedAABB.min.z);
    glVertex3f(transformedAABB.max.x, transformedAABB.max.y, transformedAABB.min.z);
    glVertex3f(transformedAABB.max.x, transformedAABB.max.y, transformedAABB.max.z);
    glVertex3f(transformedAABB.max.x, transformedAABB.max.y, transformedAABB.max.z);
    glVertex3f(transformedAABB.max.x, transformedAABB.min.y, transformedAABB.max.z);

    glEnd();
    glPopAttrib();
}

// Get the AABB in world space
AABB AABB::GetAABB(const glm::mat4& modelTransform) const
{
	glm::vec3 corners[8];
	corners[0] = glm::vec3(min.x, min.y, min.z);
	corners[1] = glm::vec3(max.x, min.y, min.z);
	corners[2] = glm::vec3(max.x, min.y, max.z);
	corners[3] = glm::vec3(min.x, min.y, max.z);
	corners[4] = glm::vec3(min.x, max.y, min.z);
	corners[5] = glm::vec3(max.x, max.y, min.z);
	corners[6] = glm::vec3(max.x, max.y, max.z);
	corners[7] = glm::vec3(min.x, max.y, max.z);

	AABB transformedAABB;
	transformedAABB.min = glm::vec3(std::numeric_limits<float>::max());
	transformedAABB.max = glm::vec3(std::numeric_limits<float>::lowest());

	for (int i = 0; i < 8; i++)
	{
		glm::vec4 corner = modelTransform * glm::vec4(corners[i], 1.0f);
		transformedAABB.min.x = std::min(transformedAABB.min.x, corner.x);
		transformedAABB.min.y = std::min(transformedAABB.min.y, corner.y);
		transformedAABB.min.z = std::min(transformedAABB.min.z, corner.z);
		transformedAABB.max.x = std::max(transformedAABB.max.x, corner.x);
		transformedAABB.max.y = std::max(transformedAABB.max.y, corner.y);
		transformedAABB.max.z = std::max(transformedAABB.max.z, corner.z);
	}

	return transformedAABB;
}