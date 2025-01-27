#pragma once
#include "Engine/App.h"
#include "Physics/PhysicsManager.h"
#include "CameraController/RacingCameraController.h"
#include "CameraController/FlyCameraController.h"


#include <imgui.h>

class MyApp : public App
{
public:
	MyApp(std::string title, int width, int height);
	~MyApp();
	void OnStart() override;
	void OnLoad(ResourceManager *rm) override;
	void OnUpdate(float deltaTime) override;
	void OnRender(Renderer* renderer) override;
	void OnImGuiRender() override;
	void OnResize(int width, int height) override;

private:
	bool m_EnterGame = false;
	int m_SelectedCamera = 0;
	bool m_controlPanel = true;

	std::unique_ptr<PhysicsManager> m_physicsManager;
	std::unique_ptr<RacingCameraController> m_cameraController;
	std::unique_ptr<FlyCameraController> m_flyCameraController;
};

