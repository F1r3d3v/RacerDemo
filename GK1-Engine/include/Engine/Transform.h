#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform {
public:
	Transform();
	virtual ~Transform();

	glm::vec3 GetWorldPosition() const;
	glm::quat GetWorldOrientation() const;
	glm::vec3 GetWorldRotation() const;
	glm::vec3 GetWorldScale() const;

	virtual void SetPosition(const glm::vec3 &position);
	virtual void Move(const glm::vec3 &offset);
	glm::vec3 GetPosition() const;

	//virtual void LookAt(const glm::vec3 &target, const glm::vec3 &up = glm::vec3(0.0f, 1.0f, 0.0f), const glm::vec3 &altUp = glm::vec3(0.0f, 0.0f, 1.0f));
	virtual void LookAt(const glm::vec3 &target, const glm::vec3 &up = glm::vec3(0.0f, 1.0f, 0.0f));

	virtual void SetRotation(const glm::vec3 &rotation);
	virtual void Rotate(const glm::vec3 &angles);
	glm::vec3 GetRotation() const;

	virtual void SetOrientation(const glm::vec3 &forward, const glm::vec3 &up);
	virtual void SetOrientation(const glm::quat &orientation);
	glm::quat GetOrientation() const;

	virtual void SetScale(const glm::vec3 &scale);
	virtual void Scale(const glm::vec3 &factor);
	glm::vec3 GetScale() const;

	glm::mat4 GetModelMatrix() const;
	void SetModelMatrix(const glm::mat4 &matrix);

	glm::mat4 GetWorldMatrix() const;
	void ApplyTransformations(const glm::mat4 &transforms);

	static glm::vec3 GetRotation(glm::mat3 rotationMatrix);
	glm::vec3 GetForward() const;
	glm::vec3 GetWorldForward() const;
	glm::vec3 GetRight() const;
	glm::vec3 GetWorldRight() const;
	glm::vec3 GetUp() const;
	glm::vec3 GetWorldUp() const;

	friend class SceneNode;

protected:
	void SetWorldMatrix(const glm::mat4 &matrix);

	glm::vec3 m_position;
	glm::quat m_orientation;
	glm::vec3 m_scale;

	glm::vec3 m_worldPosition;
	glm::quat m_worldOrientation;
	glm::vec3 m_worldScale;

	glm::mat4 m_worldMatrix;
	mutable glm::mat4 m_modelMatrix;
	mutable bool m_transformChanged;
};