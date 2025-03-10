#pragma once
#include <string>
#include <glm/glm.hpp>
#include "Window.h"
#include "Renderer.h"
#include "ResourceManager.h"

class App
{
public:
	App(std::string title, int width, int height);
	virtual ~App();

	void Run();
	void Close();

protected:
	virtual void OnStart() = 0;
	virtual void OnLoad(ResourceManager *rm) = 0;
	virtual void OnUpdate(float deltaTime) = 0;
	virtual void OnRender(Renderer* renderer) = 0;
	virtual void OnImGuiRender() = 0;
	virtual void OnResize(int width, int height) {};

	void SetVSync(bool enabled);
	bool GetVSync() const { return m_IsVsync; }
	void SetWireframe(bool enabled) { m_Renderer->SetWireframe(enabled); }
	bool GetWireframe() const { return m_Renderer->GetWireframe(); }

	bool IsFixedTimeStep = false;
	float FixedTimeStep = 1.0f / 60.0f;
	glm::ivec2 GetViewport() const;
	Renderer *GetRenderer() const { return m_Renderer; }

	Window *GetWindow() const { return m_Window; }

private:
	void Initialize();
	void FPSCounter(float deltaTime);

	bool m_IsVsync = true;
	Window *m_Window;
	Renderer *m_Renderer;
	ResourceManager *m_ResourceManager;

	int frameCount = 0;
};

