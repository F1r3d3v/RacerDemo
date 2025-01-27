#pragma once

#include "Engine/Transform.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera : public Transform {
public:

	enum class ProjectionType {
		Perspective,
		Orthographic
	};

	Camera();
	virtual ~Camera() override;

	void SetPerspective(float fov, float aspect, float nearPlane, float farPlane);
	void SetOrthographic(float size, float nearPlane, float farPlane);
	void SetFov(float fov);
	void SetViewportSize(uint32_t width, uint32_t height);

	void SetPosition(const glm::vec3 &position) override;
	void Move(const glm::vec3 &offset) override;
	void SetRotation(const glm::vec3 &rotation) override;
	void Rotate(const glm::vec3 &angles) override;
	void SetScale(const glm::vec3 &scale) override {}
	void Scale(const glm::vec3 &factor) override {}
	//void LookAt(const glm::vec3 &target, const glm::vec3 &up = glm::vec3(0.0f, 1.0f, 0.0f), const glm::vec3 &altUp = glm::vec3(0.0f, 0.0f, 1.0f)) override;
	void LookAt(const glm::vec3 &target, const glm::vec3 &up = glm::vec3(0.0f, 1.0f, 0.0f)) override;

	ProjectionType GetProjectionType() const;
	float GetOrthographicSize() const;
	float GetAspectRatio() const;
	float GetFov() const;
	float GetNearPlane() const;
	float GetFarPlane() const;
	glm::mat4 GetViewMatrix() const;
	glm::mat4 GetProjectionMatrix() const;

private:
	float m_fieldOfView;
	float m_aspectRatio;
	float m_nearPlane;
	float m_farPlane;
	float m_orthoSize;
	ProjectionType m_Type;

	mutable glm::mat4 m_viewMatrix;
	mutable glm::mat4 m_projectionMatrix;
	mutable bool m_viewChanged;
	mutable bool m_projectionChanged;
};