#pragma once

#include "Module.h"
#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/transform.hpp"
#include "ModuleInput.h"
#include "AABB.h"
#include "Frustum.h"

class ModuleCamera : public Module
{
public:
	ModuleCamera(App* app);
	~ModuleCamera();

	bool Start();
	bool Update(float dt);
	void HandleInput();
	void FrameSelected();
	bool CleanUp();

	void LookAt(const glm::vec3& spot);
	const glm::mat4& GetViewMatrix() const;
	glm::mat4 GetProjectionMatrix() const;

	void SetAsGameCamera(bool isGameCamera) { this->isGameCamera = isGameCamera; }
	bool IsGameCamera() const { return isGameCamera; }

public:
	bool isGameCamera = false;

	void HandleMovement(glm::vec3& newPos, float speed, float fastSpeed);
	void HandleZoom(float zoomSpeed);
	void HandleRotation();
	void RotateCamera(int dx, int dy);
	void CalculateViewMatrix();
	glm::vec3 RotateVector(glm::vec3 const& vector, float angle, glm::vec3 const& axis);
	void SetPosition(const glm::vec3& position);
	void SetCursor(CursorType cursorType);

	void UpdateFrustum(); // función para actualizar el frustum
	bool IsAABBVisible(const AABB& aabb); // Función para verificar visibilidad
	//void ExtractFrustumPlanes(Frustum& frustum, const glm::mat4& viewProjectionMatrix);

	Frustum frustum; // Frustum de la cámara

	glm::vec3 GetPosition();

public:
	float fov = 60.0f;
	float nearPlane = 0.1f;  // Ajustado para mejor visibilidad
	float farPlane = 1000.0f; // Aumentado para que se vea más lejos. De manera predefinida es 1000.0f. Para probar frustum cooling poner a 20.0f
	int screenWidth, screenHeight;

private:
	glm::vec3 X, Y, Z;
	glm::vec3 pos, ref;
	glm::mat4 viewMatrix;

	bool isZooming = false;
	bool isOrbiting = false;
	bool isFreeLook = false;
	bool isDragging = false;
	bool isDefaultCursor = true;
};
