#pragma once

#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include <string>
#include <vector>

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
};