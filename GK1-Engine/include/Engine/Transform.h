#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transform {
public:
	Transform();
	virtual ~Transform();

	virtual void SetPosition(const glm::vec3 &position);
	virtual void Move(const glm::vec3 &offset);
	virtual glm::vec3 GetPosition() const;

	virtual void SetRotation(const glm::vec3 &rotation);
	virtual void Rotate(const glm::vec3 &angles);
	virtual glm::vec3 GetRotation() const;

	virtual void SetScale(const glm::vec3 &scale);
	virtual void Scale(const glm::vec3 &factor);
	virtual glm::vec3 GetScale() const;

	virtual glm::mat4 GetModelMatrix() const;

	glm::vec3 GetForward() const;
	glm::vec3 GetRight() const;
	glm::vec3 GetUp() const;

protected:
	virtual void UpdateState();

	glm::vec3 m_position;
	glm::vec3 m_rotation;
	glm::vec3 m_scale;

	glm::vec3 m_forward;
	glm::vec3 m_right;
	glm::vec3 m_up;

	mutable glm::mat4 m_modelMatrix;
	mutable bool m_transformChanged;
};