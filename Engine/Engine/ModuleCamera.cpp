#include "ModuleCamera.h"
#include "App.h"
#include "Frustum.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

ModuleCamera::ModuleCamera(App* app) : Module(app)
{
	screenWidth = app->window->width;
	screenHeight = app->window->height;

	CalculateViewMatrix();

	X = glm::vec3(1.0f, 0.0f, 0.0f);
	Y = glm::vec3(0.0f, 1.0f, 0.0f);
	Z = glm::vec3(0.0f, 0.0f, 1.0f);

	pos = glm::vec3(0.0f, 5.0f, 10.0f); // Ajusta la posici�n inicial para ver m�s
	ref = glm::vec3(0.0f, 0.0f, 0.0f);
}

ModuleCamera::~ModuleCamera() {}

bool ModuleCamera::Start()
{
	LOG(LogType::LOG_INFO, "Setting up the camera");

	LookAt(ref);

	return true;
}

bool ModuleCamera::CleanUp()
{
	LOG(LogType::LOG_INFO, "Cleaning camera");

	return true;
}

bool ModuleCamera::Update(float dt)
{
	bool isMouseInside = app->editor->sceneWindow->IsMouseInside();
	if ((!isMouseInside || (!isZooming && !isFreeLook && !isOrbiting && !isDragging)) && !isDefaultCursor)
	{
		SetCursor(CursorType::DEFAULT);
	}

	if (isMouseInside)
		HandleInput();

	CalculateViewMatrix();
	UpdateFrustum();

	return true;
}

void ModuleCamera::HandleInput()
{
	glm::vec3 newPos(0, 0, 0);

	float dt = app->GetDT();
	float speed = 10.0f * dt;
	float zoomSpeed = 30.0f * dt;
	float fastSpeed = 20.0f * dt;

	HandleMovement(newPos, speed, fastSpeed);

	HandleZoom(zoomSpeed);

	HandleRotation();

	if (app->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		FrameSelected();
	}

	pos += newPos;
	ref += newPos;
}

void ModuleCamera::HandleMovement(glm::vec3& newPos, float speed, float fastSpeed)
{
	if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT
		&& app->input->GetKey(SDL_SCANCODE_LALT) == KEY_IDLE)
	{
		if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) speed = fastSpeed;

		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed;
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed;

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= X * speed;
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed;

		SetCursor(CursorType::FREELOOK);
	}
	else if (isFreeLook)
		isFreeLook = false;

	if (app->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT)
	{
		int dx = app->input->GetMouseXMotion();
		int dy = app->input->GetMouseYMotion();

		float panSpeed = 0.01f;
		newPos -= X * static_cast<float>(dx) * panSpeed;
		newPos += Y * static_cast<float>(dy) * panSpeed;

		SetCursor(CursorType::DRAG);
	}
	else if (isDragging)
		isDragging = false;
}

void ModuleCamera::HandleZoom(float zoomSpeed)
{
	int mouseZ = app->input->GetMouseZ();

	if (mouseZ != 0)
		pos -= Z * zoomSpeed * (mouseZ > 0 ? 1.0f : -1.0f);
}

void ModuleCamera::HandleRotation()
{
	int dx = -app->input->GetMouseXMotion();
	int dy = -app->input->GetMouseYMotion();

	if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT &&
		app->input->GetKey(SDL_SCANCODE_LALT) == KEY_IDLE)
	{
		RotateCamera(dx, dy);
	}

	if (app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT
		&& app->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
	{
		pos -= ref;
		RotateCamera(dx, dy);
		pos = ref + Z * glm::length(pos);
		LookAt(ref);

		SetCursor(CursorType::ORBIT);
	}
	else if (isOrbiting)
		isOrbiting = false;

	if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT
		&& app->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
	{
		float sensitivity = 0.01f;
		float zoomDelta = dy * sensitivity;
		glm::vec3 direction = glm::normalize(pos - ref);
		pos += direction * zoomDelta;

		SetCursor(CursorType::ZOOM);
	}
	else if (isZooming)
		isZooming = false;
}

void ModuleCamera::RotateCamera(int dx, int dy)
{
	float sensitivity = 0.005f;

	// Rotaci�n horizontal
	if (dx != 0)
	{
		float DeltaX = (float)dx * sensitivity;

		X = RotateVector(X, DeltaX, glm::vec3(0.0f, 1.0f, 0.0f));
		Y = RotateVector(Y, DeltaX, glm::vec3(0.0f, 1.0f, 0.0f));
		Z = RotateVector(Z, DeltaX, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	// Rotaci�n vertical (limitada para evitar que la c�mara gire completamente)
	if (dy != 0)
	{
		float deltaY = (float)dy * sensitivity;
		Y = RotateVector(Y, deltaY, X);
		Z = RotateVector(Z, deltaY, X);

		if (Y.y < 0.0f)
		{
			Z = glm::vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
			Y = glm::cross(Z, X);
		}
	}

	CalculateViewMatrix();
}

void ModuleCamera::FrameSelected()
{
	if (app->editor->selectedGameObject)
	{
		pos = glm::vec3(
			app->editor->selectedGameObject->transform->position.x,
			app->editor->selectedGameObject->transform->position.y + 10.0f,  // Ajusta la altura
			app->editor->selectedGameObject->transform->position.z + 10.0f   // Ajusta la distancia
		);
		ref = app->editor->selectedGameObject->transform->position;
		LookAt(ref);
	}
	else
	{
		pos = glm::vec3(0.0f, 5.0f, 20.0f); // Asegura que la c�mara vea bien
		ref = glm::vec3(0.0f, 0.0f, 0.0f);
		LookAt(ref);
	}
}

void ModuleCamera::LookAt(const glm::vec3& spot)
{
	ref = spot;

	Z = glm::normalize(pos - ref);
	X = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), Z));
	Y = glm::cross(Z, X);

	CalculateViewMatrix();
}

const glm::mat4& ModuleCamera::GetViewMatrix() const
{
	return viewMatrix;
}

void ModuleCamera::CalculateViewMatrix()
{
	viewMatrix = glm::mat4(
		X.x, Y.x, Z.x, 0.0f,
		X.y, Y.y, Z.y, 0.0f,
		X.z, Y.z, Z.z, 0.0f,
		-glm::dot(X, pos), -glm::dot(Y, pos), -glm::dot(Z, pos), 1.0f
	);
}

glm::mat4 ModuleCamera::GetProjectionMatrix() const
{
	float aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
	return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

glm::vec3 ModuleCamera::RotateVector(glm::vec3 const& vector, float angle, glm::vec3 const& axis)
{
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, axis);

	glm::vec4 vector4 = glm::vec4(vector, 1.0f);

	glm::vec4 rotatedVector = rotationMatrix * vector4;

	return glm::vec3(rotatedVector);
}

void ModuleCamera::SetCursor(CursorType cursorType)
{
	if (app->input->GetCursor() != cursorType)
	{
		app->input->ChangeCursor(cursorType);

		isDefaultCursor = (cursorType == CursorType::DEFAULT);
		isFreeLook = (cursorType == CursorType::FREELOOK);
		isZooming = (cursorType == CursorType::ZOOM);
		isOrbiting = (cursorType == CursorType::ORBIT);
		isDragging = (cursorType == CursorType::DRAG);
	}
}

void ModuleCamera::SetPosition(const glm::vec3& position)
{
	pos = position;
	LookAt(ref);
}

void ModuleCamera::UpdateFrustum()
{
	glm::mat4 viewMatrix = GetViewMatrix();
	glm::mat4 projectionMatrix = GetProjectionMatrix();
	glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;

	frustum.ExtractFrustumPlanes(frustum, viewProjectionMatrix);
}

bool ModuleCamera::IsAABBVisible(const AABB& aabb)
{
	return frustum.IsAABBInside(aabb);
}

//void ModuleCamera::ExtractFrustumPlanes(Frustum& frustum, const glm::mat4& viewProjectionMatrix)
//{
//	frustum.planesArray[0] = glm::vec4(
//		viewProjectionMatrix[0][3] + viewProjectionMatrix[0][0],
//		viewProjectionMatrix[1][3] + viewProjectionMatrix[1][0],
//		viewProjectionMatrix[2][3] + viewProjectionMatrix[2][0],
//		viewProjectionMatrix[3][3] + viewProjectionMatrix[3][0]);
//
//	frustum.planesArray[1] = glm::vec4(
//		viewProjectionMatrix[0][3] - viewProjectionMatrix[0][0],
//		viewProjectionMatrix[1][3] - viewProjectionMatrix[1][0],
//		viewProjectionMatrix[2][3] - viewProjectionMatrix[2][0],
//		viewProjectionMatrix[3][3] - viewProjectionMatrix[3][0]);
//
//	frustum.planesArray[2] = glm::vec4(
//		viewProjectionMatrix[0][3] + viewProjectionMatrix[0][1],
//		viewProjectionMatrix[1][3] + viewProjectionMatrix[1][1],
//		viewProjectionMatrix[2][3] + viewProjectionMatrix[2][1],
//		viewProjectionMatrix[3][3] + viewProjectionMatrix[3][1]);
//
//	frustum.planesArray[3] = glm::vec4(
//		viewProjectionMatrix[0][3] - viewProjectionMatrix[0][1],
//		viewProjectionMatrix[1][3] - viewProjectionMatrix[1][1],
//		viewProjectionMatrix[2][3] - viewProjectionMatrix[2][1],
//		viewProjectionMatrix[3][3] - viewProjectionMatrix[3][1]);
//
//	frustum.planesArray[4] = glm::vec4(
//		viewProjectionMatrix[0][3] + viewProjectionMatrix[0][2],
//		viewProjectionMatrix[1][3] + viewProjectionMatrix[1][2],
//		viewProjectionMatrix[2][3] + viewProjectionMatrix[2][2],
//		viewProjectionMatrix[3][3] + viewProjectionMatrix[3][2]);
//
//	frustum.planesArray[5] = glm::vec4(
//		viewProjectionMatrix[0][3] - viewProjectionMatrix[0][2],
//		viewProjectionMatrix[1][3] - viewProjectionMatrix[1][2],
//		viewProjectionMatrix[2][3] - viewProjectionMatrix[2][2],
//		viewProjectionMatrix[3][3] - viewProjectionMatrix[3][2]);
//}