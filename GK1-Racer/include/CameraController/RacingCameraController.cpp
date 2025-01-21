#pragma once
#include "BaseCameraController.h"

class RacingCameraController : public BaseCameraController {
public:
	struct RacingSettings {
		float followDistance{ 5.0f };        // Distance behind vehicle
		float heightOffset{ 2.0f };          // Height above vehicle
		float followStiffness{ 3.0f };       // How quickly camera catches up (higher = stiffer)
		float rotationStiffness{ 5.0f };     // How quickly camera rotates to match vehicle
		float velocityMultiplier{ 0.3f };    // How much vehicle speed affects camera distance
		float maxExtraDistance{ 3.0f };      // Maximum extra distance from speed
		float minFOV{ 60.0f };               // FOV when stationary
		float maxFOV{ 75.0f };               // FOV at high speeds
		float lookAheadFactor{ 0.3f };       // How much to look ahead during acceleration
	};

	RacingCameraController(std::shared_ptr<Camera> camera, const RacingSettings& settings = RacingSettings{})
		: BaseCameraController(camera), m_racingSettings(settings)
	{
	}

	void SetTarget(const glm::vec3 &position, const glm::vec3 &forward, const glm::vec3 &up, const glm::vec3 &velocity)
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

	void SetRacingSettings(const RacingSettings &settings)
	{
		m_racingSettings = settings;
	}

	void Update(float deltaTime) override
	{
		UpdateIdealCameraPosition();

		// Linear interpolation for position
		m_currentPosition = glm::mix(
			m_currentPosition,
			m_idealPosition,
			1.0f - std::exp(-m_racingSettings.followStiffness * deltaTime)
		);

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

	// These are no-ops for racing camera as it doesn't respond to direct input
	void OnMouseMove(double xpos, double ypos) override {}
	void OnMouseScroll(double xoffset, double yoffset) override {}
	void OnMouseButton(int button, int action, int mods) override {}

private:
	void UpdateIdealCameraPosition()
	{
		// Calculate base offset behind vehicle
		float speed = glm::length(m_targetVelocity);
		float extraDistance = glm::min(
			speed * m_racingSettings.velocityMultiplier,
			m_racingSettings.maxExtraDistance
		);

		float totalDistance = m_racingSettings.followDistance + extraDistance;

		// Calculate ideal position: behind and slightly above the vehicle
		glm::vec3 backwardOffset = -m_targetForward * totalDistance;
		glm::vec3 upOffset = m_targetUp * m_racingSettings.heightOffset;

		m_idealPosition = m_targetPosition + backwardOffset + upOffset;
	}

	RacingSettings m_racingSettings;
	glm::vec3 m_targetPosition{ 0.0f };
	glm::vec3 m_targetForward{ 0.0f, 0.0f, 1.0f };
	glm::vec3 m_targetUp{ 0.0f, 1.0f, 0.0f };
	glm::vec3 m_targetVelocity{ 0.0f };

	glm::vec3 m_currentPosition{ 0.0f };
	glm::vec3 m_idealPosition{ 0.0f };
	bool m_firstUpdate{ true };
};