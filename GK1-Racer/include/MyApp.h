#pragma once
#include "Engine/App.h"

#include <imgui.h>

class MyApp : public App
{
public:
	MyApp(std::string title, int width, int height);
	~MyApp();
	void OnStart() override;
	void OnUpdate(float deltaTime) override;
	void OnRender(Renderer* renderer) override;
	void OnImGuiRender() override;
	void OnResize(int width, int height) override;

private:
	bool m_EnterGame = false;
};

