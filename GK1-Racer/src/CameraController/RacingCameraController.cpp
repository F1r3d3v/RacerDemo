#include "CameraController/RacingCameraController.h"

RacingCameraController::RacingCameraController(std::shared_ptr<Camera> camera, const RacingSettings &settings)
	: BaseCameraController(camera), m_racingSettings(settings)
{
}

void RacingCameraController::SetTarget(const glm::vec3 &position, const glm::vec3 &forward, const glm::vec3 &up, const glm::vec3 &velocity)
{
	m_targetPosition = position;
	m_targetForward = glm::normalize(forward);
	m_targetUp = glm::normalize(up);
	m_targetVelocity = velocity;

	// Initialize camera position if it's the first update
	if (m_firstUpdate)
	{
		UpdateIdealCameraPosition();
		m_currentPosition = m_idealPosition;
		m_firstUpdate = false;
	}
}

void RacingCameraController::Update(float deltaTime)
{
	UpdateIdealCameraPosition();

	// Linear interpolation for position
	m_currentPosition = glm::mix(m_currentPosition, m_idealPosition, 1.0f - std::exp(-m_racingSettings.followStiffness * deltaTime));

	// Calculate look-at position with look-ahead based on velocity
	float speed = glm::length(m_targetVelocity);
	glm::vec3 lookAheadOffset = m_targetForward * (speed * m_racingSettings.lookAheadFactor);
	glm::vec3 lookAtPosition = m_targetPosition + lookAheadOffset;

	// Update camera position and orientation
	m_camera->SetPosition(m_currentPosition);
	m_camera->LookAt(lookAtPosition);

	// Adjust FOV based on speed
	float speedRatio = glm::clamp(speed / 30.0f, 0.0f, 1.0f); // Assuming 30 units/sec is "fast"
	float targetFOV = glm::mix(m_racingSettings.minFOV, m_racingSettings.maxFOV, speedRatio);
	m_camera->SetFov(targetFOV);
}

void RacingCameraController::UpdateIdealCameraPosition()
{
	// Calculate base offset behind vehicle
	float speed = glm::length(m_targetVelocity);
	float extraDistance = glm::min(speed * m_racingSettings.velocityMultiplier, m_racingSettings.maxExtraDistance);

	float totalDistance = m_racingSettings.followDistance + extraDistance;

	// Calculate ideal position: behind and slightly above the vehicle
	glm::vec3 backwardOffset = -m_targetForward * totalDistance;
	glm::vec3 upOffset = m_targetUp * m_racingSettings.heightOffset;

	m_idealPosition = m_targetPosition + backwardOffset + upOffset;
}
