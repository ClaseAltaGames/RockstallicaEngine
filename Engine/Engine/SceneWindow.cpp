#include "SceneWindow.h"
#include "App.h"
#include "GameObject.h"
#include "ModuleEditor.h"

SceneWindow::SceneWindow(const WindowType type, const std::string& name) : EditorWindow(type, name)
{
}

SceneWindow::~SceneWindow()
{
}

void SceneWindow::DrawWindow()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin(name.c_str(), nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar())
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
		if (ImGui::BeginMenu("Shading Mode"))
		{
			static int w = 0;
			for (int n = 0; n < 3; n++)
			{
				const char* names[] = { "Shaded", "Wireframe", "Shaded Wireframe" };
				if (ImGui::Selectable(names[n], w == n))
				{
					w = n;
					if (n == 0)
					{
						app->editor->preferencesWindow->wireframe = false;
						app->editor->preferencesWindow->shadedWireframe = false;
					}
					else if (n == 1)
					{
						app->editor->preferencesWindow->wireframe = true;
						app->editor->preferencesWindow->shadedWireframe = false;
					}
					else if (n == 2)
					{
						app->editor->preferencesWindow->wireframe = false;
						app->editor->preferencesWindow->shadedWireframe = true;
					}
				}
			}
			
			ImGui::EndMenu();
		}
		if (ImGui::Button("Play")) {
			const char* objectNames[] = { "Capsule" };
			const char* basePath = "Engine/Primitives/";
			const char* extension = ".fbx";

			for (const char* name : objectNames)
			{
				std::string fullPath = std::string(basePath) + name + extension;
				Resource* resource = app->resources->FindResourceInLibrary(fullPath, ResourceType::MODEL);
				if (!resource)
					resource = app->importer->ImportFileToLibrary(fullPath, ResourceType::MODEL);

				app->importer->modelImporter->LoadModel(resource, app->scene->root);
				app->editor->selectedGameObject = app->scene->root->children.back();
			}

			isPlaying = true;
			Update();
		}
		if (ImGui::Button("Pause")) {
			isPlaying = false;
		}

		ImGui::PopStyleVar();

		ImGui::EndMenuBar();
	}

	UpdateMouseState();

	ImVec2 textureSize = ImVec2((float)app->window->width, (float)app->window->height);
	ImVec2 windowSize = ImGui::GetWindowSize();

	ImVec2 uv0 = ImVec2((textureSize.x - windowSize.x) / (2 * textureSize.x),
		(textureSize.y + windowSize.y) / (2 * textureSize.y));
	ImVec2 uv1 = ImVec2((textureSize.x + windowSize.x) / (2 * textureSize.x),
		(textureSize.y - windowSize.y) / (2 * textureSize.y));

	ImGui::Image((void*)(intptr_t)app->renderer3D->fboTexture, windowSize, uv0, uv1);

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_FILE_PATH"))
		{
			const char* droppedFilePath = static_cast<const char*>(payload->Data);
			app->importer->ImportFile(droppedFilePath, true);
		}
		ImGui::EndDragDropTarget();
	}

	if (ImGui::IsWindowDocked() && app->editor->performanceWindow->showFpsOverlay)
	{
		if (windowSize.x > 120 && windowSize.y > 100)
		{
			float dt = app->GetDT();
			float currentFps = 1.0f / dt;
			float ms = dt * 1000.0f;

			ImVec2 windowPos = ImGui::GetWindowPos();
			ImVec2 topRightPos = ImVec2(windowPos.x + windowSize.x - 80, windowPos.y + 60);
			ImVec4 overlayColor = ImVec4(0.0f, 1.0f, 0.8f, 1.0f);
			ImGui::SetNextWindowPos(topRightPos);
			ImGui::Begin("FPSOverlay", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
			ImGui::TextColored(overlayColor, "%.2f FPS", currentFps);
			ImGui::TextColored(overlayColor, "%.2f ms", ms);
			ImGui::End();
		}
	}

	ImGui::End();
	ImGui::PopStyleVar();
}

void SceneWindow::Update()
{
	if (isPlaying)
	{
		app->scene->Update(app->GetDT());
		GameObject* capsule = app->scene->GetGameObjectByName("Capsule");
		if (capsule != nullptr)
		{
			float speed = 5.0f * app->GetDT(); // Velocidad del jugador
			float rotationSpeed = 0.1f;       // Velocidad de rotación con el mouse

			// Movimiento del Capsule con teclado
			glm::vec3 forward = capsule->transform->GetForward();
			glm::vec3 right = capsule->transform->GetRight();

			if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
				capsule->transform->position += forward * speed;
			if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
				capsule->transform->position -= forward * speed;
			if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
				capsule->transform->position -= right * speed;
			if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
				capsule->transform->position += right * speed;

			// Rotación del Capsule con el mouse
			int mouseXMotion = app->input->GetMouseXMotion();
			int mouseYMotion = app->input->GetMouseYMotion();

			if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT) // Botón derecho del ratón
			{
				capsule->transform->Rotate(glm::vec3(0.0f, 1.0f, 0.0f), -mouseXMotion * rotationSpeed); // Rotación horizontal
				app->camera->RotateCamera(mouseXMotion, mouseYMotion);
			}

			// Sincronizar la cámara con la posición de la cabeza del Capsule
			glm::vec3 cameraOffset = glm::vec3(0.0f, 1.8f, 0.0f); // Ajustar la altura a la cabeza
			app->camera->SetPosition(capsule->transform->position + cameraOffset);
		}
	}
}
