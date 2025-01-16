#pragma once

#include "Transform.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera : public Transform {
public:
	Camera();
	virtual ~Camera() override;

	void LookAt(const glm::vec3 &target, const glm::vec3 &up = glm::vec3(0.0f, 1.0f, 0.0f));

	void SetPerspective(float fov, float aspect, float nearPlane, float farPlane);
	void SetOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);
	void SetFov(float fov);
	void SetAspectRatio(float aspect);

	float GetFov() const;
	float GetNearPlane() const;
	float GetFarPlane() const;
	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetProjectionMatrix() const;

protected:
	virtual void UpdateState() override;

private:
	float m_fieldOfView;
	float m_aspectRatio;
	float m_nearPlane;
	float m_farPlane;
	bool m_isOrtho;

	mutable glm::mat4 m_viewMatrix;
	mutable glm::mat4 m_projectionMatrix;
	mutable bool m_viewChanged;
	mutable bool m_projectionChanged;
};