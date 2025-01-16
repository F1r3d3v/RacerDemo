#include "engine/Transform.h"
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/quaternion.hpp>

Transform::Transform()
	: m_position(0.0f)
	, m_rotation(0.0f)
	, m_scale(1.0f)
	, m_forward(0.0f, 0.0f, -1.0f)
	, m_right(1.0f, 0.0f, 0.0f)
	, m_up(0.0f, 1.0f, 0.0f)
	, m_modelMatrix(1.0f)
	, m_transformChanged(true)
{
}

Transform::~Transform()
{
}

void Transform::SetPosition(const glm::vec3 &position)
{
	m_position = position;
	m_transformChanged = true;
}

void Transform::Move(const glm::vec3 &offset)
{
	m_position += offset;
	m_transformChanged = true;
}

glm::vec3 Transform::GetPosition() const
{
	return m_position;
}

void Transform::SetRotation(const glm::vec3 &rotation)
{
	m_rotation = rotation;
	UpdateState();
	m_transformChanged = true;
}

void Transform::Rotate(const glm::vec3 &angles)
{
	m_rotation += angles;
	UpdateState();
	m_transformChanged = true;
}

glm::vec3 Transform::GetRotation() const
{
	return m_rotation;
}

void Transform::SetScale(const glm::vec3 &scale)
{
	m_scale = scale;
	m_transformChanged = true;
}

void Transform::Scale(const glm::vec3 &factor)
{
	m_scale *= factor;
	m_transformChanged = true;
}

glm::vec3 Transform::GetScale() const
{
	return m_scale;
}

void Transform::UpdateState()
{
	glm::vec3 radians = glm::radians(m_rotation);
	glm::mat4 rotationMatrix = glm::eulerAngleYXZ(radians.y, radians.x, radians.z);

	m_forward = -glm::vec3(rotationMatrix[2]);
	m_right = glm::vec3(rotationMatrix[0]);
	m_up = glm::vec3(rotationMatrix[1]);
}

glm::mat4 Transform::GetModelMatrix() const
{
	if (m_transformChanged)
	{
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_position);

		glm::vec3 radians = glm::radians(m_rotation);
		glm::mat4 rotation = glm::eulerAngleYXZ(radians.y, radians.x, radians.z);

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_scale);

		m_modelMatrix = translation * rotation * scale;
		m_transformChanged = false;
	}
	return m_modelMatrix;
}

glm::vec3 Transform::GetForward() const
{
	return m_forward;
}
glm::vec3 Transform::GetRight() const
{
	return m_right;
}
glm::vec3 Transform::GetUp() const
{
	return m_up;
}