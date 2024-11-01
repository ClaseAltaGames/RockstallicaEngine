#pragma once

#include "EditorWindow.h"

#include <filesystem>
#include <vector>
#include <string>

#include <GL/glew.h>

class ProjectWindow : public EditorWindow
{
public:
    ProjectWindow(const WindowType type, const std::string& name);
    ~ProjectWindow();

    void DrawWindow() override;

    void UpdateDirectoryContent();
    std::vector<std::string> GetPathParts() const;

    GLuint LoadTexture(const std::string& filePath);

private:
    std::filesystem::path currentPath;
    std::vector<std::filesystem::directory_entry> directoryContents;

    GLuint folderIcon;
    GLuint fileIcon;
};