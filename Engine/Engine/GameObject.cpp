#include "GameObject.h"

GameObject::GameObject(const char* name, GameObject* parent)
    : parent(parent), name(name),
    boundingBox(AABB(glm::vec3(0.0f), glm::vec3(0.0f)))
{
    transform = new ComponentTransform(this);
    mesh = new ComponentMesh(this);
    material = new ComponentMaterial(this);

    AddComponent(transform);
    AddComponent(mesh);
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
        if (mesh)
        {
            mesh->GenerateBoundingBox();
        }

        // Combinar bounding boxes de los hijos
        GenerateCombinedBoundingBox();
    }
}

void GameObject::GenerateCombinedBoundingBox()
{
    if (!mesh || mesh->vertices.empty())
    {
        // Si no hay malla, inicializa una bounding box vacía
        boundingBox = AABB(glm::vec3(0.0f), glm::vec3(0.0f));
    }
    else
    {
        // Usa la bounding box de la malla del GameObject
        boundingBox = mesh->boundingBox;
    }

    // Expandir la bounding box con las de los hijos
    for (auto* child : children)
    {
        if (child->mesh)
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
        mesh = static_cast<ComponentMesh*>(component);
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
    if (!mesh || mesh->vertices.empty())
    {
        // Si no hay malla, devuelve un JSON indicando que no hay malla
        return { {"hasMesh", false} };
    }

    // Serializa los vértices
    json verticesJson = json::array();
    for (const auto& vertex : mesh->vertices)
    {
        verticesJson.push_back({
            {"x", vertex.x},
            {"y", vertex.y},
            {"z", vertex.z}
            });
    }

    // Serializa los índices
    json indicesJson = json::array();
    for (const auto& index : mesh->indices)
    {
        indicesJson.push_back(index);
    }

    // Serializa las normales
    json normalsJson = json::array();
    for (const auto& normal : mesh->normals)
    {
        normalsJson.push_back({
            {"x", normal.x},
            {"y", normal.y},
            {"z", normal.z}
            });
    }

    // Devuelve un JSON con los datos de la malla
    return {
        {"hasMesh", true},
        {"vertices", verticesJson},
        {"indices", indicesJson},
        {"normals", normalsJson}
    };
}
json GameObject::SerializeMaterial() const
{
    // Implementa la serialización del material según tus necesidades
    return {
        {"texture_path", material->materialTexture ? material->materialTexture->texturePath : ""}
    };
}

void GameObject::DeserializeFromJson(const json& gameObjectJson)
{
    // Restaurar nombre
    name = gameObjectJson["name"];

    // Restaurar transformación
    if (gameObjectJson.contains("transform")) {
        const auto& transformJson = gameObjectJson["transform"];
        transform->position = glm::vec3(
            transformJson["position"][0],
            transformJson["position"][1],
            transformJson["position"][2]
        );
        transform->rotation = glm::vec3(
            transformJson["rotation"][0],
            transformJson["rotation"][1],
            transformJson["rotation"][2]
        );
        transform->scale = glm::vec3(
            transformJson["scale"][0],
            transformJson["scale"][1],
            transformJson["scale"][2]
        );
    }

    // Restaurar malla
    if (gameObjectJson.contains("mesh")) {
        const auto& meshJson = gameObjectJson["mesh"];

        // Asegurar que la malla exista
        if (!mesh) {
            mesh = new ComponentMesh(this);
        }

        // Limpiar malla existente
        mesh->vertices.clear();
        mesh->indices.clear();
        mesh->normals.clear();

        // Restaurar vértices
        if (meshJson.contains("vertices") && !meshJson["vertices"].empty()) {
            for (const auto& vertJson : meshJson["vertices"]) {
                mesh->vertices.push_back(glm::vec3(
                    vertJson["x"].get<float>(),
                    vertJson["y"].get<float>(),
                    vertJson["z"].get<float>()
                ));
            }
        }

        // Restaurar índices
        if (meshJson.contains("indices") && !meshJson["indices"].empty()) {
            mesh->indices = meshJson["indices"].get<std::vector<uint32_t>>();
        }

        // Restaurar normales
        if (meshJson.contains("normals") && !meshJson["normals"].empty()) {
            for (const auto& normalJson : meshJson["normals"]) {
                mesh->normals.push_back(glm::vec3(
                    normalJson["x"].get<float>(),
                    normalJson["y"].get<float>(),
                    normalJson["z"].get<float>()
                ));
            }
        }
    }

    // Restaurar material si existe
    if (gameObjectJson.contains("material")) {
        const auto& materialJson = gameObjectJson["material"];
        if (materialJson.contains("texture_path") && !materialJson["texture_path"].get<std::string>().empty()) {
            // Cargar textura si es necesario
            // Esto dependerá de cómo manejes la carga de texturas en tu motor
            // Podrías hacer algo como:
            // Texture* texture = app->textureLoader->LoadTexture(materialJson["texture_path"]);
            //material->AddTexture(texture);
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