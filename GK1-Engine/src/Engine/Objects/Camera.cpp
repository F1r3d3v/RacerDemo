#include "Engine/Objects/Camera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

Camera::Camera() : Transform()
, m_fieldOfView(45.0f)
, m_aspectRatio(16.0f/9.0f)
, m_nearPlane(0.1f)
, m_farPlane(1000.0f)
, m_orthoSize(50.0f)
, m_viewMatrix(1.0f)
, m_projectionMatrix(1.0f)
, m_viewChanged(true)
, m_projectionChanged(true)
, m_Type(ProjectionType::Perspective)
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
	m_Type = ProjectionType::Perspective;
	m_fieldOfView = fov;
	m_aspectRatio = aspect;
	m_nearPlane = nearPlane;
	m_farPlane = farPlane;
	m_projectionChanged = true;
}

void Camera::SetOrthographic(float size, float nearPlane, float farPlane)
{
	m_Type = ProjectionType::Orthographic;
	m_orthoSize = size;
	m_nearPlane = nearPlane;
	m_farPlane = farPlane;
	m_projectionChanged = true;
}

void Camera::SetFov(float fov)
{
	m_fieldOfView = fov;
	m_projectionChanged = true;
}

void Camera::SetViewportSize(uint32_t width, uint32_t height)
{
	m_aspectRatio = (float)width / height;
	m_projectionChanged = true;
}

void Camera::SetPosition(const glm::vec3 &position)
{
	Transform::SetPosition(position);
	m_viewChanged = true;
}

void Camera::Move(const glm::vec3 &offset)
{
	Transform::Move(offset);
	m_viewChanged = true;
}

void Camera::SetRotation(const glm::vec3 &rotation)
{
	Transform::SetRotation(rotation);
	m_viewChanged = true;
}

void Camera::Rotate(const glm::vec3 &angles)
{
	Transform::Rotate(angles);
	m_viewChanged = true;
}

void Camera::LookAt(const glm::vec3 &target, const glm::vec3 &up, const glm::vec3 &altUp)
{
	Transform::LookAt(target, up, altUp);
	m_viewChanged = true;
}

Camera::ProjectionType Camera::GetProjectionType() const
{
	return m_Type;
}

float Camera::GetOrthographicSize() const
{
	return m_orthoSize;
}

float Camera::GetAspectRatio() const
{
	return m_aspectRatio;
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
		if (m_Type == ProjectionType::Perspective)
		{
			m_projectionMatrix = glm::perspective(glm::radians(m_fieldOfView), m_aspectRatio, m_nearPlane, m_farPlane);
		}
		else if (m_Type == ProjectionType::Orthographic)
		{
			float orthoLeft = -m_orthoSize * m_aspectRatio * 0.5f;
			float orthoRight = m_orthoSize * m_aspectRatio * 0.5f;
			float orthoBottom = -m_orthoSize * 0.5f;
			float orthoTop = m_orthoSize * 0.5f;
			m_projectionMatrix = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_nearPlane, m_farPlane);
		}
		m_projectionChanged = false;
	}
	return m_projectionMatrix;
}