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

void ModuleScene::SaveScene(const char* path)
{
    json sceneJson;

    // Función recursiva para serializar los GameObjects
    std::function<json(GameObject*)> SaveGameObject = [&](GameObject* gameObject) -> json {
        json gameObjectJson;

        // Guardar propiedades básicas
        gameObjectJson["name"] = gameObject->name;
       /* gameObjectJson["position"] = { gameObject->position.x, gameObject->position.y, gameObject->position.z };
        gameObjectJson["rotation"] = { gameObject->rotation.x, gameObject->rotation.y, gameObject->rotation.z };
        gameObjectJson["scale"] = { gameObject->scale.x, gameObject->scale.y, gameObject->scale.z };*/

        // Guardar Mesh si tiene una
        if (gameObject->mesh) {
            // Convertir vértices de float3 a std::vector<std::vector<float>>
            std::vector<std::vector<float>> serializedVertices;
            for (const auto& vertex : gameObject->mesh->GetVertices()) {
                serializedVertices.push_back({ vertex.x, vertex.y, vertex.z });
            }

            // Convertir normales de float3 a std::vector<std::vector<float>>
            std::vector<std::vector<float>> serializedNormals;
            for (const auto& normal : gameObject->mesh->GetNormals()) {
                serializedNormals.push_back({ normal.x, normal.y, normal.z });
            }

            // Serializar índices (si ya es std::vector<uint32_t>, no necesita conversión)
            std::vector<uint32_t> serializedIndices = gameObject->mesh->GetIndices();

            // Guardar en JSON
            gameObjectJson["mesh"]["vertices"] = serializedVertices;
            gameObjectJson["mesh"]["indices"] = serializedIndices;
            gameObjectJson["mesh"]["normals"] = serializedNormals;
        }

        // Guardar hijos
        for (GameObject* child : gameObject->children)
        {
            gameObjectJson["children"].push_back(SaveGameObject(child));
        }

        return gameObjectJson;
        };

    // Guardar la raíz de la escena
    for (GameObject* child : root->children)
    {
        sceneJson["gameObjects"].push_back(SaveGameObject(child));
    }

    // Escribir el JSON al archivo
    std::string scenePath = std::string(path) + ".scene";
    std::ofstream file(scenePath);
    if (file.is_open())
    {
        file << sceneJson.dump(4); // Con 4 espacios de indentación para legibilidad
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

                // Si el objeto tiene una malla
                if (gameObjectJson.contains("mesh"))
                {
                    const auto& meshJson = gameObjectJson["mesh"];

                    // Crear un componente de malla para este GameObject
                    ComponentMesh* mesh = new ComponentMesh();

                    // Cargar vértices
                    if (meshJson.contains("vertices"))
                    {
                        for (const auto& vertex : meshJson["vertices"])
                        {
                            glm::vec3 vert;
                            vert.x = vertex[0];
                            vert.y = vertex[1];
                            vert.z = vertex[2];
                            mesh->vertices.push_back(vert);
                        }
                    }

                    // Cargar índices
                    if (meshJson.contains("indices"))
                    {
                        for (const auto& index : meshJson["indices"])
                        {
                            mesh->indices.push_back(index);
                        }
                    }

                    // Cargar normales
                    if (meshJson.contains("normals"))
                    {
                        for (const auto& normal : meshJson["normals"])
                        {
                            glm::vec3 norm;
                            norm.x = normal[0];
                            norm.y = normal[1];
                            norm.z = normal[2];
                            mesh->normals.push_back(norm);
                        }
                    }

                    // Asignar el componente de malla al GameObject
                    gameObject->mesh = mesh;
                }

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