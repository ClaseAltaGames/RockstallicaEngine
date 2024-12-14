#include "GameObject.h"

GameObject::GameObject(const char* name, GameObject* parent) : parent(parent), name(name)
{
	transform = new ComponentTransform(this);
	mesh = new ComponentMesh(this);
	material = new ComponentMaterial(this);

	AddComponent(transform);
	AddComponent(mesh);
	//AddComponent(material);
}

GameObject::~GameObject()
{
}

void GameObject::Update()
{
	if (isActive)
	{
		for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		{
			(*it)->Update();
		}
		for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it)
		{
			(*it)->Update();
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