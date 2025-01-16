#include "engine/Camera.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

Camera::Camera() : Transform()
, m_fieldOfView(45.0f)
, m_aspectRatio(1.0f)
, m_nearPlane(0.1f)
, m_farPlane(1000.0f)
, m_viewMatrix(1.0f)
, m_projectionMatrix(1.0f)
, m_viewChanged(true)
, m_projectionChanged(true)
{
}

Camera::~Camera()
{
}

void Camera::UpdateState()
{
	Transform::UpdateState();
	m_viewChanged = true;
}

void Camera::LookAt(const glm::vec3 &target, const glm::vec3 &up)
{
	glm::mat4 lookAt = glm::lookAt(m_position, target, up);

	glm::mat3 rotationMatrix(lookAt);
	glm::vec3 angles = glm::degrees(glm::eulerAngles(glm::quat_cast(rotationMatrix)));

	SetRotation(-angles);
}

glm::mat4 Camera::GetViewMatrix() const
{
	if (m_viewChanged)
	{
		m_viewMatrix = glm::lookAt(m_position, m_position + m_forward, m_up);
		m_viewChanged = false;
	}
	return m_viewMatrix;
}

void Camera::SetPerspective(float fov, float aspect, float nearPlane, float farPlane)
{
	m_isOrtho = false;
	m_fieldOfView = fov;
	m_aspectRatio = aspect;
	m_nearPlane = nearPlane;
	m_farPlane = farPlane;
	m_projectionChanged = true;
}

void Camera::SetOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane)
{
	m_isOrtho = true;
	m_nearPlane = nearPlane;
	m_farPlane = farPlane;
	m_projectionMatrix = glm::ortho(left, right, bottom, top, nearPlane, farPlane);
	m_projectionChanged = true;
}

void Camera::SetFov(float fov)
{
	m_fieldOfView = fov;
	m_projectionChanged = true;
}

void Camera::SetAspectRatio(float aspect)
{
	m_aspectRatio = aspect;
	m_projectionChanged = true;
}

float Camera::GetFov() const
{
	return m_fieldOfView;
}
float Camera::GetNearPlane() const
{
	return m_nearPlane;
}
float Camera::GetFarPlane() const
{
	return m_farPlane;
}

glm::mat4 Camera::GetProjectionMatrix() const
{
	if (m_projectionChanged)
	{
		m_projectionMatrix = glm::perspective(glm::radians(m_fieldOfView), m_aspectRatio, m_nearPlane, m_farPlane);
		m_projectionChanged = false;
	}
	return m_projectionMatrix;
}