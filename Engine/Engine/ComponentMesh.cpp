#define NOMINMAX
#include "ComponentMesh.h"
#include "App.h"
#include <glm/gtc/type_ptr.hpp>
#include <nlohmann/json.hpp>
#include <algorithm>
#include <limits>

using json = nlohmann::json;

// Constructor
ComponentMesh::ComponentMesh(GameObject* gameObject)
    : Component(gameObject, ComponentType::MESH),
    mesh(nullptr),
    boundingBox(AABB(glm::vec3(0.0f), glm::vec3(0.0f))),  // AABB inicializada
    showBoundingBox(true)  // Nueva variable para controlar la visibilidad de AABB
{
}

// Destructor
ComponentMesh::~ComponentMesh() {}

void ComponentMesh::Update() {
    ComponentTransform* transform = gameObject->transform;

    if (transform != nullptr) {
        // Configurar la matriz de transformaci�n global para el objeto
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

        if (showVertexNormals || showFaceNormals) {
            mesh->DrawNormals(
                showVertexNormals,
                showFaceNormals,
                app->editor->preferencesWindow->vertexNormalLength,
                app->editor->preferencesWindow->faceNormalLength,
                app->editor->preferencesWindow->vertexNormalColor,
                app->editor->preferencesWindow->faceNormalColor
            );
        }

        // Aseg�rate de que la AABB est� actualizada antes de dibujarla
        boundingBox = mesh->GetBoundingBox();  // Actualiza la AABB

        // Dibujar la caja delimitadora AABB si est� habilitada
        if (showBoundingBox) {
            if (app->editor->selectedGameObject == gameObject) {
                boundingBox.DrawAABB(transform->globalTransform);
            }
        }
    }

    if (transform != nullptr) glPopMatrix();
}

void ComponentMesh::OnEditor() {
    if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (mesh) {
            ImGui::Text("Vertices: %d", mesh->verticesCount);
            ImGui::Text("Indices: %d", mesh->indicesCount);
            ImGui::Text("Normals: %d", mesh->normalsCount);
            ImGui::Text("Texture Coords: %d", mesh->texCoordsCount);

            ImGui::Spacing();
            ImGui::Text("Bounding Box:");
            ImGui::Text("Min: (%.2f, %.2f, %.2f)",
                boundingBox.min.x, boundingBox.min.y, boundingBox.min.z);
            ImGui::Text("Max: (%.2f, %.2f, %.2f)",
                boundingBox.max.x, boundingBox.max.y, boundingBox.max.z);

            ImGui::Spacing();

            ImGui::Checkbox("Vertex Normals", &showVertexNormals);
            ImGui::Checkbox("Face Normals", &showFaceNormals);

            ImGui::Checkbox("Show Bounding Box", &showBoundingBox);
        }
    }
}

void ComponentMesh::GenerateBoundingBox() {
    if (!mesh || mesh->verticesCount == 0) {
        boundingBox = AABB(
            glm::vec3(std::numeric_limits<float>::max()),
            glm::vec3(std::numeric_limits<float>::lowest())
        );
        return;
    }

    glm::vec3 minPoint = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 maxPoint = glm::vec3(std::numeric_limits<float>::lowest());

    for (uint i = 0; i < mesh->verticesCount; ++i) {
        glm::vec3 vertex = glm::make_vec3(&mesh->vertices[i * 3]);
        minPoint.x = std::min(minPoint.x, vertex.x);
        minPoint.y = std::min(minPoint.y, vertex.y);
        minPoint.z = std::min(minPoint.z, vertex.z);
        
        maxPoint.x = std::max(maxPoint.x, vertex.x);
        maxPoint.y = std::max(maxPoint.y, vertex.y);
        maxPoint.z = std::max(maxPoint.z, vertex.z);
    }

    boundingBox = AABB(minPoint, maxPoint);
}

bool ComponentMesh::IsPointInBoundingBox(const glm::vec3& point) const {
    return point.x >= boundingBox.min.x && point.x <= boundingBox.max.x &&
        point.y >= boundingBox.min.y && point.y <= boundingBox.max.y &&
        point.z >= boundingBox.min.z && point.z <= boundingBox.max.z;
}
