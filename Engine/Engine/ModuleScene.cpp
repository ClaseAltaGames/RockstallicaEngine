#define NOMINMAX


#include "ModuleScene.h"
#include "App.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include "glm/glm.hpp"
#include "RayCast.h"

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
    if (app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN) {
        SelectObjectFromMouse();
    }
   /* if (app->editor->selectedGameObject) {
        app->editor->selectedGameObject->boundingBox->DrawAABB(app->editor->selectedGameObject->transform);
    }*/
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
    sceneJson["name"] = root->name;
    sceneJson["gameObjects"] = json::array();

    // Guardar cada hijo del root como un gameobject en el JSON
    for (GameObject* child : root->children) {
        sceneJson["gameObjects"].push_back(child->SerializeToJson());
    }

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

        // Limpiar hijos existentes
        root->children.clear();

        // Restaurar nombre de la escena
        if (sceneJson.contains("name")) {
            root->name = sceneJson["name"];
        }

        // Cargar GameObjects
        if (sceneJson.contains("gameObjects")) {
            for (const auto& gameObjectJson : sceneJson["gameObjects"]) {
                GameObject* newGameObject = new GameObject(
                    gameObjectJson["name"].get<std::string>().c_str(),
                    root
                );
                newGameObject->DeserializeFromJson(gameObjectJson);
                root->children.push_back(newGameObject);
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

// In ModuleScene.cpp, modify SelectObjectFromMouse():

void ModuleScene::SelectObjectFromMouse()
{
    // Clear previous selection state first
    if (app->editor->selectedGameObject) {
        app->editor->selectedGameObject->componentMesh->showBoundingBox = false;
    }

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    Ray ray = GenerateRayFromMouse(mouseX, mouseY, app->camera->GetViewMatrix(), app->camera->GetProjectionMatrix());

    RayCast rayCast;
    GameObject* closestObject = nullptr;
    float closestDistance = std::numeric_limits<float>::max();

    std::function<void(GameObject*)> checkObject = [&](GameObject* obj) {
        if (obj->componentMesh && obj->componentMesh->mesh) {
            AABB worldBoundingBox = obj->componentMesh->boundingBox.GetAABB(obj->transform->globalTransform);

            if (rayCast.IntersectsAABB(ray.origin, ray.direction, worldBoundingBox.min, worldBoundingBox.max)) {
                float distance = glm::length(worldBoundingBox.min - ray.origin);
                if (distance < closestDistance) {
                    closestDistance = distance;
                    closestObject = obj;
                }
            }
        }

        for (auto* child : obj->children) {
            checkObject(child);
        }
        };

    checkObject(root);

    if (closestObject) {
        app->editor->selectedGameObject = closestObject;
        closestObject->componentMesh->showBoundingBox = true;
        LOG(LogType::LOG_INFO, "Selected object: %s", closestObject->name.c_str());
    }
}
Ray ModuleScene::GenerateRayFromMouse(int mouseX, int mouseY, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
    int width, height;
    SDL_GetWindowSize(app->window->window, &width, &height);

    // Convertir coordenadas del ratón a NDC
    float x = (2.0f * mouseX) / width - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / height;
    glm::vec4 rayNDC(x, y, -1.0f, 1.0f);

    // Convertir de NDC a espacio de vista
    glm::vec4 rayView = glm::inverse(projectionMatrix) * rayNDC;
    rayView.z = -1.0f;
    rayView.w = 0.0f;

    // Convertir de espacio de vista a espacio del mundo
    glm::vec3 rayWorld = glm::vec3(glm::inverse(viewMatrix) * rayView);
    rayWorld = glm::normalize(rayWorld);

    return Ray(app->camera->GetPosition(), rayWorld);
}
