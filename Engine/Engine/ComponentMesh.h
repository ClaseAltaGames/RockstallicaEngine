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

    std::vector<glm::vec3> vertices;  // Vertex coordinates
    std::vector<uint32_t> indices;    // Triangle indices
    std::vector<glm::vec3> normals;   // Normals

    const std::vector<glm::vec3>& GetVertices() { return vertices; }
    const std::vector<uint32_t>& GetIndices() { return indices; }
    const std::vector<glm::vec3>& GetNormals() { return normals; }

    void Save(json& meshJson);

    // New AABB-related methods
    void GenerateBoundingBox();
    AABB GetBoundingBox() const { return boundingBox; }
    bool IsPointInBoundingBox(const glm::vec3& point) const;

public:
    Mesh* mesh;
    AABB boundingBox;  // Bounding box for the mesh

private:

    bool showVertexNormals = false;
    bool showFaceNormals = false;
};