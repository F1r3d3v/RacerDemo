#pragma once
#include <string>
#include <glm/glm.hpp>
#include "Window.h"
#include "Renderer.h"

class App
{
public:
	App(std::string title, int width, int height);
	virtual ~App();

	void Run();
	void Close();

protected:
	virtual void OnStart() = 0;
	virtual void OnUpdate(float deltaTime) = 0;
	virtual void OnRender(Renderer* renderer) = 0;
	virtual void OnImGuiRender() = 0;
	virtual void OnResize(int width, int height) {};

	void SetVSync(bool enabled);
	bool GetVSync() const { return m_IsVsync; }

	bool IsFixedTimeStep = false;
	float FixedTimeStep = 1.0f / 60.0f;
	glm::ivec2 GetViewport() const;
	Renderer *GetRenderer() const { return m_Renderer; }

private:
	void Initialize();

	bool m_IsVsync = true;
	Window *m_Window;
	Renderer *m_Renderer;
};

