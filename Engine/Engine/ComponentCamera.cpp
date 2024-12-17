// ComponentCamera.cpp
#include "ComponentCamera.h"
#include "imgui.h"
#include "glm/gtc/matrix_transform.hpp"
#include "App.h"

ComponentCamera::ComponentCamera(GameObject* owner) : Component(owner, ComponentType::CAMERA)
{
    position = glm::vec3(0.0f, 5.0f, 10.0f);
    reference = glm::vec3(0.0f, 0.0f, 0.0f);
}

ComponentCamera::~ComponentCamera() {}

void ComponentCamera::Update(float dt)
{
    // Opcional: Actualizar posición/dirección si la cámara está "activa".
}

void ComponentCamera::OnEditor()
{
    if (ImGui::CollapsingHeader("Camera"))
    {
        if (ImGui::DragFloat("FOV", &fov, 1.0f, 1.0f, 120.0f))
        {
            // Actualiza algo si el FOV cambia.
        }

        if (ImGui::DragFloat("Near Plane", &nearPlane, 0.01f, 0.01f, farPlane))
        {
            // Actualiza near plane.
        }

        if (ImGui::DragFloat("Far Plane", &farPlane, 1.0f, nearPlane, 10000.0f))
        {
            // Actualiza far plane.
        }

        ImGui::Separator();
        ImGui::Text("Position:");
        ImGui::DragFloat3("##Position", &position.x, 0.1f);
    }
}

void ComponentCamera::LookAt(const glm::vec3& target)
{
    reference = target;
}

glm::mat4 ComponentCamera::GetViewMatrix() const
{
    return glm::lookAt(position, reference, glm::vec3(0, 1, 0));
}

glm::mat4 ComponentCamera::GetProjectionMatrix() const
{
    float aspectRatio = 16.0f / 9.0f; // Obtener de la ventana
    return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}