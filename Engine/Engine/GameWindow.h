#pragma once

#include "EditorWindow.h"

class GameWindow : public EditorWindow
{
public:
	GameWindow(const WindowType type, const std::string& name);
	~GameWindow();

	void DrawWindow() override;
};