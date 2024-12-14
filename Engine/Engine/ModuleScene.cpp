#include "ModuleScene.h"
#include "App.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include "glm/glm.hpp"

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




#include <nlohmann/json.hpp>
#include <glm/vec3.hpp>

void to_json(nlohmann::json& j, const glm::vec3& vec) {
    j = nlohmann::json{ {"x", vec.x}, {"y", vec.y}, {"z", vec.z} };
}

void from_json(const nlohmann::json& j, glm::vec3& vec) {
    j.at("x").get_to(vec.x);
    j.at("y").get_to(vec.y);
    j.at("z").get_to(vec.z);
}


void ModuleScene::SaveScene(const char* path)
{
    json sceneJson;

    // Guardar el nombre de la escena
    sceneJson["name"] = root->name; 

    // Función recursiva para guardar GameObjects
    std::function<json(GameObject*)> SaveGameObject = [&](GameObject* gameObject) -> json {
        json gameObjectJson;
        gameObjectJson["name"] = gameObject->name;

        // Guardar transformaciones (posición, rotación, escala)
        gameObjectJson["position"] = { gameObject->transform->position.x, gameObject->transform->position.y, gameObject->transform->position.z };
        gameObjectJson["rotation"] = { gameObject->transform->rotation.x, gameObject->transform->rotation.y, gameObject->transform->rotation.z };
        gameObjectJson["scale"] = { gameObject->transform->scale.x, gameObject->transform->scale.y, gameObject->transform->scale.z };

        // Guardar Mesh si tiene un ComponentMesh
       // Guardar Mesh si tiene un ComponentMesh
        json meshJson;
        if (gameObject->mesh) {
            gameObject->mesh->Save(meshJson);
        }
        gameObjectJson["mesh"] = meshJson;

        // Guardar hijos recursivamente
        for (GameObject* child : gameObject->children) {
            gameObjectJson["children"].push_back(SaveGameObject(child));
        }

        return gameObjectJson;
        };

    // Guardar los hijos de la raíz
    for (GameObject* child : root->children) {
        sceneJson["gameObjects"].push_back(SaveGameObject(child));
    }

    // Guardar en archivo
    std::string scenePath = std::string(path) + ".scene";
    std::ofstream file(scenePath);
    if (file.is_open()) {
        file << sceneJson.dump(4);
        file.close();
        LOG(LogType::LOG_INFO, "Scene saved to %s", scenePath.c_str());
    }
    else {
        LOG(LogType::LOG_ERROR, "Failed to save scene to %s", scenePath.c_str());
    }
}

void ModuleScene::LoadScene(const char* path)
{
    std::string scenePath = std::string(path) + ".scene";
    std::ifstream file(scenePath);
    if (file.is_open()) {
        json sceneJson;
        file >> sceneJson;
        file.close();

        root->children.clear();

        // Cargar el nombre de la escena
        if (sceneJson.contains("name")) {
            root->name = sceneJson["name"];
        }

        if (sceneJson.contains("gameObjects")) {
            // Función recursiva para cargar los GameObjects
            std::function<void(json&, GameObject*)> LoadGameObject = [&](json& gameObjectJson, GameObject* parent) {
                const std::string& name = gameObjectJson["name"];
                GameObject* gameObject = CreateGameObject(name.c_str(), parent);

                // Cargar transformaciones (posición, rotación, escala)
                if (gameObjectJson.contains("position")) {
                    gameObject->transform->position = glm::vec3(gameObjectJson["position"][0], gameObjectJson["position"][1], gameObjectJson["position"][2]);
                }
                if (gameObjectJson.contains("rotation")) {
                    gameObject->transform->rotation = glm::vec3(gameObjectJson["rotation"][0], gameObjectJson["rotation"][1], gameObjectJson["rotation"][2]);
                }
                if (gameObjectJson.contains("scale")) {
                    gameObject->transform->scale = glm::vec3(gameObjectJson["scale"][0], gameObjectJson["scale"][1], gameObjectJson["scale"][2]);
                }

                // Cargar Mesh si existe
                // Cargar Mesh si existe
                if (gameObjectJson.contains("mesh") && gameObjectJson["mesh"].contains("has_mesh")) {
                    const auto& meshJson = gameObjectJson["mesh"];
                    ComponentMesh* mesh = new ComponentMesh(gameObject);
                
                    if (meshJson.contains("vertices")) {
                        for (const auto& vertex : meshJson["vertices"]) {
                            glm::vec3 vert(vertex[0], vertex[1], vertex[2]);
                            mesh->vertices.push_back(vert);
                        }
                    }

                    if (meshJson.contains("indices")) {
                        for (const auto& index : meshJson["indices"]) {
                            mesh->indices.push_back(index);
                        }
                    }

                    if (meshJson.contains("normals")) {
                        for (const auto& normal : meshJson["normals"]) {
                            glm::vec3 norm(normal[0], normal[1], normal[2]);
                            mesh->normals.push_back(norm);
                        }
                    }

                    gameObject->mesh = mesh;
                }

                // Cargar hijos si existen
                if (gameObjectJson.contains("children")) {
                    for (json& childJson : gameObjectJson["children"]) {
                        LoadGameObject(childJson, gameObject);
                    }
                }
                };

            // Procesar los GameObjects de la raíz
            for (json& gameObjectJson : sceneJson["gameObjects"]) {
                LoadGameObject(gameObjectJson, root);
            }

            LOG(LogType::LOG_INFO, "Scene loaded from %s", scenePath.c_str());
        }
        else {
            LOG(LogType::LOG_ERROR, "No gameObjects found in %s", scenePath.c_str());
        }
    }
    else {
        LOG(LogType::LOG_ERROR, "Failed to load scene from %s", scenePath.c_str());
    }
}
