#include "Engine/Transform.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

Transform::Transform()
	: m_position(0.0f)
	, m_worldPosition(0.0f)
	, m_rotation(0.0f)
	, m_worldRotation(0.0f)
	, m_scale(1.0f)
	, m_worldScale(1.0f)
	, m_forward(0.0f, 0.0f, -1.0f)
	, m_right(1.0f, 0.0f, 0.0f)
	, m_up(0.0f, 1.0f, 0.0f)
	, m_modelMatrix(1.0f)
	, m_worldMatrix(1.0f)
	, m_transformChanged(true)
{
}

Transform::~Transform()
{
}

glm::vec3 Transform::GetWorldPosition() const
{
	return m_worldPosition;
}

glm::vec3 Transform::GetWorldRotation() const
{
	return m_worldRotation;
}

glm::vec3 Transform::GetWorldScale() const
{
	return m_worldScale;
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

void Transform::LookAt(const glm::vec3 &target, const glm::vec3 &up, const glm::vec3 &altUp)
{
	glm::vec3 direction = glm::normalize(target - m_position);
	glm::vec3 right;
	float dot = glm::dot(direction, up);
	if (glm::abs(dot) > .9999f)
		right = glm::normalize(glm::cross(direction, (dot < 0.0f) ? -altUp : altUp));
	else
		right = glm::normalize(glm::cross(direction, up));
	glm::vec3 newUp = glm::cross(right, direction);

	glm::mat3 rotationMatrix = glm::inverse(glm::mat3(glm::lookAt(m_position, m_position + direction, newUp)));
	m_rotation = GetRotation(rotationMatrix);

	m_forward = -glm::vec3(rotationMatrix[2]);
	m_right = glm::vec3(rotationMatrix[0]);
	m_up = glm::vec3(rotationMatrix[1]);

	m_transformChanged = true;
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
	glm::mat4 rotationMatrix = glm::eulerAngleYXZ(-radians.y, radians.x, radians.z);

	m_forward = -glm::vec3(rotationMatrix[2]);
	m_right = glm::vec3(rotationMatrix[0]);
	m_up = glm::vec3(rotationMatrix[1]);
}

glm::vec3 Transform::GetRotation(glm::mat3 rotationMatrix) const
{
	glm::vec3 rotation;

	rotation.y = std::atan2(-rotationMatrix[0][2], rotationMatrix[0][0]);
	rotation.x = -std::atan2(-rotationMatrix[1][2], std::sqrt(rotationMatrix[0][2] * rotationMatrix[0][2] + rotationMatrix[2][2] * rotationMatrix[2][2]));
	rotation.z = std::atan2(rotationMatrix[1][0], rotationMatrix[1][1]);

	return glm::degrees(rotation);
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

glm::mat4 Transform::GetWorldMatrix() const
{
	return m_worldMatrix;
}

void Transform::SetWorldMatrix(const glm::mat4 &matrix)
{
	if (m_worldMatrix == matrix) return;
	m_worldMatrix = matrix;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::quat rotation;
	glm::decompose(matrix, m_worldScale, rotation, m_worldPosition, skew, perspective);
	m_worldRotation = glm::degrees(glm::eulerAngles(rotation));
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