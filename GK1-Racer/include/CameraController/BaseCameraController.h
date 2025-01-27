#pragma once
#include "Engine/Objects/Camera.h"
#include "Engine/Window.h"
#include <glm/glm.hpp>
#include <memory>

class BaseCameraController {
public:
	struct Settings {
		float mouseSensitivity{ 0.1f };
		bool invertY{ false };
	};

	BaseCameraController(std::shared_ptr<Camera> camera, const Settings &settings = Settings{})
		: m_camera(camera), m_mouseSensitivity(settings.mouseSensitivity), m_invertY(settings.invertY) {
	}

	virtual ~BaseCameraController() = default;

	virtual void Update(float deltaTime) = 0;

	void SetCamera(std::shared_ptr<Camera> camera) { m_camera = camera; }
	std::shared_ptr<Camera> GetCamera() const { return m_camera; }

protected:
	std::shared_ptr<Camera> m_camera;

	float m_mouseSensitivity;
	bool m_invertY;
	float m_pitch{ 0.0f };
	float m_yaw{ 0.0f };
};