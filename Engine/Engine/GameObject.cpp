#include "GameObject.h"
#include "ComponentMesh.h"

GameObject::GameObject(const char* name, GameObject* parent)
    : parent(parent), name(name),
    boundingBox(AABB(glm::vec3(0.0f), glm::vec3(0.0f)))
{


    transform = new ComponentTransform(this);
    componentMesh = new ComponentMesh(this);
    material = new ComponentMaterial(this);

    AddComponent(transform);
    AddComponent(componentMesh);
}

GameObject::~GameObject()
{
}

void GameObject::Update()
{
    if (isActive)
    {
        // Actualizar componentes
        for (auto* component : components)
        {
            component->Update();
        }

        // Actualizar hijos
        for (auto* child : children)
        {
            child->Update();
        }

        // Generar bounding box si hay una malla asociada
        if (componentMesh)
        {
            componentMesh->GenerateBoundingBox();
        }

        // Combinar bounding boxes de los hijos
        GenerateCombinedBoundingBox();
    }
}

void GameObject::GenerateCombinedBoundingBox()
{
    if (!componentMesh || !componentMesh->mesh || componentMesh->mesh->verticesCount == 0)
    {
        // Si no hay malla o no hay vértices, inicializa una bounding box vacía
        boundingBox = AABB(glm::vec3(0.0f), glm::vec3(0.0f));
    }
    else
    {
        // Usa la bounding box de la malla del GameObject
        boundingBox = componentMesh->boundingBox;
    }

    // Expandir la bounding box con las de los hijos
    for (auto* child : children)
    {
        if (child->componentMesh && child->componentMesh->mesh && child->componentMesh->mesh->verticesCount > 0)
        {
            boundingBox.min.x = std::min(boundingBox.min.x, child->boundingBox.min.x);
            boundingBox.min.y = std::min(boundingBox.min.y, child->boundingBox.min.y);
            boundingBox.min.z = std::min(boundingBox.min.z, child->boundingBox.min.z);

            boundingBox.max.x = std::max(boundingBox.max.x, child->boundingBox.max.x);
            boundingBox.max.y = std::max(boundingBox.max.y, child->boundingBox.max.y);
            boundingBox.max.z = std::max(boundingBox.max.z, child->boundingBox.max.z);
        }
    }
}


void GameObject::Enable()
{
    isActive = true;
}
void GameObject::Disable()
{
    isActive = false;
}

void GameObject::DeleteGameObject(GameObject* gameObject)
{
    if (!gameObject) return;

    // Recursively delete children first
    while (!gameObject->children.empty())
    {
        DeleteGameObject(gameObject->children.back());
    }

    // Safely delete components
    for (auto it = gameObject->components.begin(); it != gameObject->components.end(); )
    {
        if (*it)  // Additional null check
        {
            delete* it;
            it = gameObject->components.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // Remove from parent's children
    if (gameObject->parent)
    {
        auto it = std::find(gameObject->parent->children.begin(),
            gameObject->parent->children.end(),
            gameObject);
        if (it != gameObject->parent->children.end())
        {
            gameObject->parent->children.erase(it);
        }
    }

    delete gameObject;
}

void GameObject::DeleteComponent(Component* component)
{
    if (!component) return;

    auto it = std::find(components.begin(), components.end(), component);
    if (it != components.end())
    {
        components.erase(it);
    }

    delete component;
}

Component* GameObject::AddComponent(Component* component)
{
	components.push_back(component);

    // Establecer el puntero mesh si el componente es de tipo MESH
    if (component->type == ComponentType::MESH)
    {
        componentMesh = static_cast<ComponentMesh*>(component);
    }

	return component;
}

Component* GameObject::GetComponent(ComponentType type)
{
	for (auto it = components.begin(); it != components.end(); ++it) {
		if ((*it)->type == type) {
			return (*it);
		}
	}

	return nullptr;
}

json GameObject::SerializeToJson() const
{
    json gameObjectJson;
    gameObjectJson["name"] = name;

    // Serializar transformación
    gameObjectJson["transform"] = SerializeTransform();

    // Serializar malla si existe
    gameObjectJson["mesh"] = SerializeMesh();

    // Serializar material si existe
    gameObjectJson["material"] = SerializeMaterial();

    // Serializar Bounding Box
    gameObjectJson["bounding_box"] = {
        {"min", {boundingBox.min.x, boundingBox.min.y, boundingBox.min.z}},
        {"max", {boundingBox.max.x, boundingBox.max.y, boundingBox.max.z}}
    };

    // Serializar hijos
    if (!children.empty()) {
        json childrenJson = json::array();
        for (const auto* child : children) {
            childrenJson.push_back(child->SerializeToJson());
        }
        gameObjectJson["children"] = childrenJson;
    }

    return gameObjectJson;
}


json GameObject::SerializeTransform() const
{
    return {
        {"position", {transform->position.x, transform->position.y, transform->position.z}},
        {"rotation", {transform->rotation.x, transform->rotation.y, transform->rotation.z}},
        {"scale", {transform->scale.x, transform->scale.y, transform->scale.z}}
    };
}

json GameObject::SerializeMesh() const
{
    // Verifica si hay una malla asignada al GameObject
    if (!componentMesh->mesh)
    {
        // Si no hay malla, devuelve un JSON indicando que no hay malla
        return { {"hasMesh", false} };
    }

    // Serializa los vértices

    json verticesJson = json::array();
	for (int varCoord = 0; varCoord < componentMesh->mesh->verticesCount * 3; varCoord+= 3) {
        verticesJson.push_back({
            {"x", componentMesh->mesh->vertices[varCoord] },
            {"y", componentMesh->mesh->vertices[varCoord + 1]},
            {"z", componentMesh->mesh->vertices[varCoord + 2]}
            });
    }

    // Serializa los índices
    json indicesJson = json::array();
	for (int i = 0; i < componentMesh->mesh->indicesCount * 3; i += 3) {
		indicesJson.push_back({
            componentMesh->mesh->indices[i],
            componentMesh->mesh->indices[i + 1],
            componentMesh->mesh->indices[i + 2]
			});
	}

    // Serializa las normales
    json normalsJson = json::array();
	for (int normalsCoord = 0; normalsCoord < componentMesh->mesh->normalsCount * 3; normalsCoord += 3) {
		normalsJson.push_back({
			{"x", componentMesh->mesh->normals[normalsCoord]},
			{"y", componentMesh->mesh->normals[normalsCoord + 1]},
			{"z", componentMesh->mesh->normals[normalsCoord + 2]}
			});
	}

	json uvJson = json::array();
	for (int uvCoord = 0; uvCoord < componentMesh->mesh->texCoordsCount * 2; uvCoord += 2) {
		uvJson.push_back({
			{"u", componentMesh->mesh->texCoords[uvCoord]},
			{"v", componentMesh->mesh->texCoords[uvCoord + 1]}
			});
	}
        
    // Devuelve un JSON con los datos de la malla
    return {
        {"hasMesh", true},
        {"vertices", verticesJson},
        {"indices", indicesJson},
		{"normals", normalsJson},
		{"uv", uvJson}
    };
}
json GameObject::SerializeMaterial() const
{
	// Verifica si hay un material asignado al GameObject
	if (!material->materialTexture)
	{
		// Si no hay material, devuelve un JSON indicando que no hay material
		return { {"hasMaterial", false} };
	}

	// Devuelve un JSON con la ruta de la textura, el size y el id
	return {
		{"hasMaterial", true},
		{"texture_path", material->materialTexture->texturePath},
		{"texture_size", {material->materialTexture->textureWidth, material->materialTexture->textureHeight}},
		{"texture_id", material->materialTexture->textureId}
	};
}

void GameObject::DeserializeFromJson(const json& gameObjectJson)
{
    // Restaurar nombre
    name = gameObjectJson["name"];

    // Restaurar transformación
    if (gameObjectJson.contains("transform")) {
        const auto& transformJson = gameObjectJson["transform"];

        // Asegúrate de que el transform existe
        if (!transform) {
            transform = new ComponentTransform(this);
            components.push_back(transform);
        }

        // Asignar los valores de la transformación
        if (transformJson.contains("position")) {
            transform->position = glm::vec3(
                transformJson["position"][0].get<float>(),
                transformJson["position"][1].get<float>(),
                transformJson["position"][2].get<float>()
            );
        }
        if (transformJson.contains("rotation")) {
            transform->rotation = glm::vec3(
                transformJson["rotation"][0].get<float>(),
                transformJson["rotation"][1].get<float>(),
                transformJson["rotation"][2].get<float>()
            );
        }
        if (transformJson.contains("scale")) {
            transform->scale = glm::vec3(
                transformJson["scale"][0].get<float>(),
                transformJson["scale"][1].get<float>(),
                transformJson["scale"][2].get<float>()
            );
        }
		transform->UpdateTransform();
    }


    if (gameObjectJson.contains("mesh")) {
        const auto& meshJson = gameObjectJson["mesh"];

        // Asegurar que la malla exista
        //if (!componentMesh->mesh) {
        componentMesh = new ComponentMesh(this);
        componentMesh->mesh = new Mesh();
        components.push_back(componentMesh);
        // }

         // Restaurar vértices
        if (meshJson.contains("vertices") && !meshJson["vertices"].empty()) {
            int countVertices = meshJson["vertices"].size();
            int indexVertices = 0;
            componentMesh->mesh->vertices = (float*)malloc(sizeof(float) * countVertices * 3);
            for (const auto& vertexJson : meshJson["vertices"]) {
                componentMesh->mesh->vertices[indexVertices] = vertexJson["x"].get<float>();
                componentMesh->mesh->vertices[indexVertices + 1] = vertexJson["y"].get<float>();
                componentMesh->mesh->vertices[indexVertices + 2] = vertexJson["z"].get<float>();
                indexVertices += 3;
            }
            componentMesh->mesh->verticesCount = countVertices;
        }

        // Restaurar índices
        if (meshJson.contains("indices") && !meshJson["indices"].empty()) {
            int countIndicies = meshJson["indices"].size();
            int indexIndicies = 0;
            componentMesh->mesh->indices = (uint*)malloc(sizeof(uint) * countIndicies * 3);
            for (const auto& indexJson : meshJson["indices"]) {
                componentMesh->mesh->indices[indexIndicies] = indexJson[0].get<uint>();
                componentMesh->mesh->indices[indexIndicies + 1] = indexJson[1].get<uint>();
                componentMesh->mesh->indices[indexIndicies + 2] = indexJson[2].get<uint>();
                indexIndicies += 3;
            }
            componentMesh->mesh->indicesCount = countIndicies;
        }

        // Restaurar normales
        if (meshJson.contains("normals") && !meshJson["normals"].empty()) {
            int countNormals = meshJson["normals"].size();
            int indexNormals = 0;
            componentMesh->mesh->normals = (float*)malloc(sizeof(float) * countNormals * 3);
            for (const auto& normalJson : meshJson["normals"]) {
                componentMesh->mesh->normals[indexNormals] = normalJson["x"].get<float>();
                componentMesh->mesh->normals[indexNormals + 1] = normalJson["y"].get<float>();
                componentMesh->mesh->normals[indexNormals + 2] = normalJson["z"].get<float>();
                indexNormals += 3;
            }
            componentMesh->mesh->normalsCount = countNormals;
        }

        // Restaurar UVs
        if (meshJson.contains("uv") && !meshJson["uv"].empty()) {
            int countUVs = meshJson["uv"].size();
            int indexUVs = 0;
            componentMesh->mesh->texCoords = (float*)malloc(sizeof(float) * countUVs * 2);
            for (const auto& uvJson : meshJson["uv"]) {
                componentMesh->mesh->texCoords[indexUVs] = uvJson["u"].get<float>();
                componentMesh->mesh->texCoords[indexUVs + 1] = uvJson["v"].get<float>();
                indexUVs += 2;
            }
            componentMesh->mesh->texCoordsCount = countUVs;
        }

        componentMesh->mesh->InitMesh();

        // Restaurar material si existe
        if (gameObjectJson.contains("material")) {
            const auto& materialJson = gameObjectJson["material"];

            if (materialJson["hasMaterial"].get<bool>()) {
                // Obtener el path de la textura
                std::string texturePathStr = materialJson["texture_path"].get<std::string>();
                const char* newTexturePath = texturePathStr.c_str();

                // Obtener el tamaño de la textura
                int textureWidth = materialJson["texture_size"][0].get<int>();
                int textureHeight = materialJson["texture_size"][1].get<int>();

                // Obtener el ID de la textura
                uint textureId = materialJson["texture_id"].get<uint>();

                // Crear la textura con los valores deserializados
                material->materialTexture = new Texture(textureId, textureWidth, textureHeight, newTexturePath);
                material->AddTexture(material->materialTexture);

            }
        }
            // Restaurar hijos
            if (gameObjectJson.contains("children")) {
                for (const auto& childJson : gameObjectJson["children"]) {
                    GameObject* child = new GameObject(childJson["name"].get<std::string>().c_str(), this);
                    child->DeserializeFromJson(childJson);
                    children.push_back(child);
                }
            }
    }
}