#pragma once
#include <vector>
#include "Component.h"
#include "Mesh.h"
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

	std::vector<glm::vec3> vertices;  // Coordenadas de los vértices
	std::vector<uint32_t> indices; // Índices de los triángulos
	std::vector<glm::vec3> normals;   // Normales

	
	const std::vector<glm::vec3>& GetVertices() { return vertices; }
	const std::vector<uint32_t>& GetIndices()  { return indices; }
	const std::vector<glm::vec3>& GetNormals()  { return normals; }

	void Save(json& meshJson);


public:
	Mesh* mesh;

private:
	bool showVertexNormals = false;
	bool showFaceNormals = false;
};