#pragma once

#include "Module.h"
#include "GameObject.h"

class GameObject;

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

	std::vector<GameObject*> GetAllGameObjects();


public:
	GameObject* root = nullptr;
};