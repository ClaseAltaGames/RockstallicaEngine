#pragma once

#include "Module.h"
#include "GameObject.h"

class GameObject;

struct Ray {
	glm::vec3 origin;
	glm::vec3 direction;

	Ray(const glm::vec3& origin, const glm::vec3& direction)
		: origin(origin), direction(glm::normalize(direction)) {}
};

class ModuleScene : public Module
{
public:
	ModuleScene(App* app);
	virtual ~ModuleScene();

	bool Awake();
	bool Update(float dt);
	bool CleanUp();

	GameObject* CreateGameObject(const char* name, GameObject* parent);
	//crear la funcion GetGameObjectByName
	GameObject* GetGameObjectByName(const char* name);

	//save scene
	void SaveScene(const char* path);

	//load scene
	void LoadScene(const char* path);

	void SelectObjectFromMouse();

	Ray GenerateRayFromMouse(int mouseX, int mouseY, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

public:
	GameObject* root = nullptr;
};

