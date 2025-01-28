#include "CameraController/RacingCameraController.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

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

	if (m_firstUpdate)
	{
		UpdateTargetCameraPosition();
		m_currentPosition = m_idealPosition;
		m_firstUpdate = false;
	}
}

void RacingCameraController::Update(float deltaTime)
{
	UpdateTargetCameraPosition();

	m_currentPosition = glm::mix(m_currentPosition, m_idealPosition, m_racingSettings.followStiffness * deltaTime);

	float speed = glm::length(m_targetVelocity);
	if (speed < 0.2f)
		speed = 0.0f;
	glm::vec3 lookAheadOffset = m_targetForward * (speed * m_racingSettings.lookAheadFactor);
	glm::vec3 lookAtPosition = m_targetPosition + lookAheadOffset;

	glm::quat targetOrientation = glm::quatLookAt(glm::normalize(lookAtPosition - m_currentPosition), m_targetUp);
	m_currentOrientation = glm::slerp(m_currentOrientation, targetOrientation, m_racingSettings.strafeStiffness * deltaTime);

	m_camera->SetPosition(m_currentPosition);
	m_camera->SetOrientation(m_currentOrientation);

	float speedRatio = glm::clamp(speed / 30.0f, 0.0f, 1.0f);
	float targetFOV = glm::mix(m_racingSettings.minFOV, m_racingSettings.maxFOV, speedRatio);
	m_camera->SetFov(targetFOV);
}

void RacingCameraController::UpdateTargetCameraPosition()
{
	float speed = glm::length(m_targetVelocity);
	float extraDistance = glm::min(speed * m_racingSettings.velocityMultiplier, m_racingSettings.maxExtraDistance);

	float totalDistance = m_racingSettings.followDistance + extraDistance;

	glm::vec3 backwardOffset = -m_targetForward * totalDistance;
	glm::vec3 upOffset = m_targetUp * m_racingSettings.heightOffset;

	m_idealPosition = m_targetPosition + backwardOffset + upOffset;
}
