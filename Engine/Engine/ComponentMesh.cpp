#include "ComponentMesh.h"
#include "App.h"
#include <glm/gtc/type_ptr.hpp>
#include <nlohmann/json.hpp> // Incluir la librería JSON
using json = nlohmann::json; // Definir alias para json

ComponentMesh::ComponentMesh(GameObject* gameObject)
    : Component(gameObject, ComponentType::MESH), mesh(nullptr)
{
}

ComponentMesh::~ComponentMesh()
{
}

void ComponentMesh::Update()
{
    ComponentTransform* transform = gameObject->transform;

    if (transform != nullptr)
    {
        glPushMatrix();
        glMultMatrixf(glm::value_ptr(transform->globalTransform));
    }

    ComponentMaterial* material = gameObject->material;

    if (mesh) {
        mesh->DrawMesh(
            material->textureId,
            app->editor->preferencesWindow->drawTextures,
            app->editor->preferencesWindow->wireframe,
            app->editor->preferencesWindow->shadedWireframe
        );

        if (showVertexNormals || showFaceNormals)
        {
            mesh->DrawNormals(
                showVertexNormals,
                showFaceNormals,
                app->editor->preferencesWindow->vertexNormalLength,
                app->editor->preferencesWindow->faceNormalLength,
                app->editor->preferencesWindow->vertexNormalColor,
                app->editor->preferencesWindow->faceNormalColor
            );
        }
    }

    if (transform != nullptr) glPopMatrix();
}

void ComponentMesh::OnEditor()
{
    if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (mesh) {
            ImGui::Text("Vertices: %d", mesh->verticesCount);
            ImGui::Text("Indices: %d", mesh->indicesCount);
            ImGui::Text("Normals: %d", mesh->normalsCount);
            ImGui::Text("Texture Coords: %d", mesh->texCoordsCount);

            ImGui::Spacing();

            ImGui::Checkbox("Vertex Normals", &showVertexNormals);
            ImGui::Checkbox("Face Normals", &showFaceNormals);
        }
    }
}

void ComponentMesh::Save(json& meshJson)
{
    // Indica que la mesh no es nula
    meshJson["has_mesh"] = true;

    // Verifica que los vectores tengan datos antes de guardarlos
    if (!vertices.empty()) {
        for (const auto& vertex : vertices) {
            meshJson["vertices"].push_back({ {"x", vertex.x}, {"y", vertex.y}, {"z", vertex.z} });
        }
    }

    if (!indices.empty()) {
        meshJson["indices"] = indices;
    }

    if (!normals.empty()) {
        for (const auto& normal : normals) {
            meshJson["normals"].push_back({ {"x", normal.x}, {"y", normal.y}, {"z", normal.z} });
        }
    }
}