// ComponentCamera.h
#pragma once
#include "Component.h"
#include "glm/glm.hpp"
#include "ModuleImporter.h"

class ComponentCamera : public Component
{
public:
    ComponentCamera(GameObject* owner);
    ~ComponentCamera() override;

    void Update(float dt);
    void OnEditor() override; // Mostrar opciones en el Inspector
    void LookAt(const glm::vec3& target);

    void SetFOV(float fov);
    void SetNearPlane(float nearPlane);
    void SetFarPlane(float farPlane);

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;

    Icons icon;

public:
    float fov = 60.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;

private:
    glm::vec3 position;
    glm::vec3 reference;
};