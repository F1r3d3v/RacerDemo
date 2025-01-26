#pragma once
#include "Engine/Objects/Camera.h"
#include "Engine/Window.h"
#include <glm/glm.hpp>
#include <memory>

class BaseCameraController {
public:
	struct Settings {
		float moveSpeed{ 5.0f };
		float mouseSensitivity{ 0.1f };
		bool invertY{ false };
	};

	BaseCameraController(std::shared_ptr<Camera> camera)
		: m_camera(camera)
	{
	}

	virtual ~BaseCameraController() = default;

	virtual void Update(float deltaTime) = 0;
	virtual void OnMouseMove(double xpos, double ypos) = 0;
	virtual void OnMouseScroll(double xoffset, double yoffset) = 0;
	virtual void OnMouseButton(int button, int action, int mods) = 0;

	void SetCamera(std::shared_ptr<Camera> camera) { m_camera = camera; }
	std::shared_ptr<Camera> GetCamera() const { return m_camera; }

	void SetSettings(const Settings &settings) { m_settings = settings; }
	const Settings &GetSettings() const { return m_settings; }

protected:
	std::shared_ptr<Camera> m_camera;
	Settings m_settings;

	struct State {
		glm::vec3 target{ 0.0f };
		float distance{ 10.0f };
		float pitch{ 0.0f };
		float yaw{ 0.0f };
		float targetDistance{ 10.0f };

		glm::vec2 lastMousePos{ 0.0f };
		bool mousePressed{ false };
		bool rightMousePressed{ false };

		glm::vec3 velocity{ 0.0f };
		glm::vec3 angularVelocity{ 0.0f };
	} m_state;

	glm::vec3 CalculatePanDelta(const glm::vec2 &delta) const
	{
		float panScale = m_state.distance * 0.001f;
		return m_camera->GetRight() * delta.x * panScale + m_camera->GetUp() * delta.y * panScale;
	}
};