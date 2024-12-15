#pragma once
#include "glm/glm.hpp"

glm::vec3 GetRayFromScreenCoords(int mouseX, int mouseY, int screenWidth, int screenHeight, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);