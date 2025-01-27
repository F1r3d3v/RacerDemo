#include "CameraController/FlyCameraController.h"
#include "Engine/Input.h"

FlyCameraController::FlyCameraController(std::shared_ptr<Camera> camera, const FlySettings &settings)
	: BaseCameraController(camera, settings), m_moveSpeed(settings.moveSpeed)
{
}

void FlyCameraController::Update(float deltaTime)
{
	glm::vec3 forward = m_camera->GetForward();
	glm::vec3 right = m_camera->GetRight();
	glm::vec3 up = m_camera->GetUp();

	glm::vec3 cameraSpeed = glm::vec3(m_moveSpeed * deltaTime);
	if (Input::IsKeyDown(GLFW_KEY_LEFT_SHIFT))
		cameraSpeed *= 10.0f;
	if (Input::IsKeyDown(GLFW_KEY_LEFT_CONTROL))
		cameraSpeed *= 0.5f;
	if (Input::IsKeyDown(GLFW_KEY_W))
		m_camera->Move(forward * cameraSpeed);
	if (Input::IsKeyDown(GLFW_KEY_S))
		m_camera->Move(-forward * cameraSpeed);
	if (Input::IsKeyDown(GLFW_KEY_A))
		m_camera->Move(-right * cameraSpeed);
	if (Input::IsKeyDown(GLFW_KEY_D))
		m_camera->Move(right * cameraSpeed);
	if (Input::IsKeyDown(GLFW_KEY_Q))
		m_camera->Move(-up * cameraSpeed);
	if (Input::IsKeyDown(GLFW_KEY_E))
		m_camera->Move(up * cameraSpeed);

	double xpos, ypos;
	Input::GetMouseDelta(xpos, ypos);
	glm::vec2 mouseDelta(xpos, ypos);
	mouseDelta *= m_mouseSensitivity;

	// Invert Y
	if (m_invertY)
		mouseDelta.y *= -1;

	// Accumulate pitch and yaw	
	m_pitch += mouseDelta.y;
	m_yaw += mouseDelta.x;

	// Clamp pitch
	if (m_pitch > 89.0f)
		m_pitch = 89.0f;

	if (m_pitch < -89.0f)
		m_pitch = -89.0f;

	m_camera->SetRotation(glm::vec3(-m_pitch, -m_yaw, 0.0f));
}
