#pragma once

#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include <string>
#include <vector>
#include "AABB.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class GameObject
{
public:
    GameObject(const char* name, GameObject* parent = nullptr);
    virtual ~GameObject();

    void Update();
    void Enable();
    void Disable();
	bool IsEnabled() { return isActive; }

    void DeleteGameObject(GameObject* gameObject);
    void DeleteComponent(Component* component);

    Component* AddComponent(Component* component);
    Component* GetComponent(ComponentType type);

    template <typename T>
    T* GetComponent();

	json SerializeToJson() const;
    void DeserializeFromJson(const json& gameObjectJson);


public:
    GameObject* parent;
    std::string name;

    ComponentTransform* transform;
    ComponentMesh* mesh;
    ComponentMaterial* material;

    std::vector<Component*> components;
    std::vector<GameObject*> children;

    bool isActive = true;
    bool isEditing = false;

private:
	json SerializeTransform() const;
	json SerializeMesh() const;
	json SerializeMaterial() const;


public:
template <>
AABB* GetComponent<AABB>() {
    for (Component* component : components) {
        if (AABB* aabb = dynamic_cast<AABB*>(component)) {
            return aabb;
        }
    }
    return nullptr;
}

};