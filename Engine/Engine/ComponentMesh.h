#pragma once
#include <vector>
#include "Component.h"
#include "Mesh.h"
#include "AABB.h"
#include "glm/glm.hpp"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

class Mesh;

class ComponentMesh : public Component
{
public:
    ComponentMesh(GameObject* gameObject);
    virtual ~ComponentMesh();

    void Update() override;
    void OnEditor() override;

    // New AABB-related methods
    void GenerateBoundingBox();
    AABB GetBoundingBox() const { return boundingBox; }
    bool IsPointInBoundingBox(const glm::vec3& point) const;

public:
    Mesh* mesh;
    AABB boundingBox;  // Bounding box for the mesh
	bool showBoundingBox = false;

private:

    bool showVertexNormals = false;
    bool showFaceNormals = false;
};