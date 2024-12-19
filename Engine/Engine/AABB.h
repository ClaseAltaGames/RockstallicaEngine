#pragma once
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <algorithm>

class AABB {
public:
    glm::vec3 min;
    glm::vec3 max;

    // Constructor por defecto
    AABB();

    // Constructor con parámetros
    AABB(const glm::vec3& min, const glm::vec3& max);

    // Métodos
    bool Intersects(const AABB& other) const;
    void Update(const glm::vec3& point);

	//Draw the bounding box
	void DrawAABB(const glm::mat4& modelTransform) const;

	//get the AABB in world space
	AABB GetAABB(const glm::mat4& modelTransform) const;
};

