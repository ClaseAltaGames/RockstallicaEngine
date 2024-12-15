#include "GetRayFromScreenCoords.h"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/matrix_transform.hpp"

// Function to convert screen coordinates to a ray in world space
glm::vec3 GetRayFromScreenCoords(int mouseX, int mouseY, int screenWidth, int screenHeight, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    // Convert screen coordinates to normalized device coordinates
    float x = (2.0f * mouseX) / screenWidth - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / screenHeight;
    glm::vec4 rayNDC(x, y, -1.0f, 1.0f);

    // Convert NDC to clip space
    glm::vec4 rayClip(rayNDC.x, rayNDC.y, -1.0f, 1.0f);

    // Convert clip space to eye space
    glm::mat4 invProjectionMatrix = glm::inverse(projectionMatrix);
    glm::vec4 rayEye = invProjectionMatrix * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    // Convert eye space to world space
    glm::mat4 invViewMatrix = glm::inverse(viewMatrix);
    glm::vec4 rayWorld = invViewMatrix * rayEye;
    glm::vec3 rayDirection = glm::normalize(glm::vec3(rayWorld));

    return glm::vec3();
}
