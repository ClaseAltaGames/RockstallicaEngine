#include "ModuleScene.h"
#include "App.h"
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;


ModuleScene::ModuleScene(App* app) : Module(app), root(nullptr)
{
}

ModuleScene::~ModuleScene()
{
}

bool ModuleScene::Awake()
{
	root = CreateGameObject("Untitled Scene", nullptr);

	return true;
}

bool ModuleScene::Update(float dt)
{
	root->Update();

	//Hide Objects
	if (app->input->GetKey(SDL_SCANCODE_H) == KEY_DOWN && app->editor->selectedGameObject != root)
	{
		if (app->editor->selectedGameObject->IsEnabled())
			app->editor->selectedGameObject->Disable();
		else
			app->editor->selectedGameObject->Enable();
	}
	//Delete Objects
	if (app->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN && app->editor->selectedGameObject != root)
	{
		auto& siblings = app->editor->selectedGameObject->parent->children;
		siblings.erase(std::remove(siblings.begin(), siblings.end(), app->editor->selectedGameObject), siblings.end());
	}

	return true;
}

bool ModuleScene::CleanUp()
{
	LOG(LogType::LOG_INFO, "Cleaning ModuleScene");

	return true;
}

GameObject* ModuleScene::CreateGameObject(const char* name, GameObject* parent)
{
	GameObject* gameObject = new GameObject(name, parent);

	if (parent != nullptr) parent->children.push_back(gameObject);

	return gameObject;
}

GameObject* ModuleScene::GetGameObjectByName(const char* name)
{
	//check if there is a Capsule Game Object in the scene
	for (auto it = root->children.begin(); it != root->children.end(); ++it) {
		if ((*it)->name == name) {
			return (*it);
		}
	}

	return nullptr;
}

void ModuleScene::SaveScene(const char* path)
{
    // Crear el objeto JSON principal
    json sceneJson;

    // Guardar los objetos de la escena recursivamente
    std::function<void(GameObject*, json&)> SaveGameObject = [&](GameObject* gameObject, json& parentJson) {
        // Crear un objeto JSON para este GameObject
        json gameObjectJson;
        gameObjectJson["name"] = gameObject->name;
       /* gameObjectJson["position"] = { gameObject->position.x, gameObject->position.y, gameObject->position.z };
        gameObjectJson["rotation"] = { gameObject->rotation.x, gameObject->rotation.y, gameObject->rotation.z };
        gameObjectJson["scale"] = { gameObject->scale.x, gameObject->scale.y, gameObject->scale.z };*/

        // Recorrer y guardar los hijos
        for (GameObject* child : gameObject->children)
        {
            SaveGameObject(child, gameObjectJson["children"]);
        }

        // Añadir este GameObject al JSON padre
        parentJson.push_back(gameObjectJson);
        };

    // Llamar a la función recursiva para guardar desde la raíz
    SaveGameObject(root, sceneJson["gameObjects"]);

    // Guardar el JSON en el archivo
    std::string scenePath = std::string(path) + ".scene";
    std::ofstream file(scenePath);
    if (file.is_open())
    {
        file << sceneJson.dump(4); // Formateado con una sangría de 4 espacios
        file.close();
        LOG(LogType::LOG_INFO, "Scene saved to %s", scenePath.c_str());
    }
    else
    {
        LOG(LogType::LOG_ERROR, "Failed to save scene to %s", scenePath.c_str());
    }
}

void ModuleScene::LoadScene(const char* path)
{
    // Cargar el archivo JSON
    std::string scenePath = std::string(path) + ".scene";
    std::ifstream file(scenePath);
    if (file.is_open())
    {
        json sceneJson;
        file >> sceneJson;
        file.close();

        // Limpiar la escena actual
        root->children.clear();

        // Si hay objetos en el JSON
        if (sceneJson.contains("gameObjects"))
        {
            json& rootObject = sceneJson["gameObjects"][0]; // Asignar el primer objeto como raíz
            root->name = rootObject["name"]; // Cambiar el nombre del root a "Escena"

            // Cargar los hijos de "Escena"
            std::function<void(json&, GameObject*)> LoadGameObject = [&](json& gameObjectJson, GameObject* parent) {
                // Crear un GameObject hijo
                const std::string& name = gameObjectJson["name"];
                GameObject* gameObject = CreateGameObject(name.c_str(), parent);

                // Recorrer y cargar los hijos
                if (gameObjectJson.contains("children"))
                {
                    for (json& childJson : gameObjectJson["children"])
                    {
                        LoadGameObject(childJson, gameObject);
                    }
                }
                };

            // Procesar los hijos de la raíz
            if (rootObject.contains("children"))
            {
                for (json& childJson : rootObject["children"])
                {
                    LoadGameObject(childJson, root);
                }
            }

            LOG(LogType::LOG_INFO, "Scene loaded from %s", scenePath.c_str());
        }
        else
        {
            LOG(LogType::LOG_ERROR, "No gameObjects found in %s", scenePath.c_str());
        }
    }
    else
    {
        LOG(LogType::LOG_ERROR, "Failed to load scene from %s", scenePath.c_str());
    }
}