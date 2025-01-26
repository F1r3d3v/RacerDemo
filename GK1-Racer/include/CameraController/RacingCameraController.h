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

	RacingCameraController(std::shared_ptr<Camera> camera, const RacingSettings &settings = RacingSettings{});
	~RacingCameraController() = default;

	void SetTarget(const glm::vec3 &position, const glm::vec3 &forward, const glm::vec3 &up, const glm::vec3 &velocity);
	void SetRacingSettings(const RacingSettings &settings) { m_racingSettings = settings; }

	void Update(float deltaTime) override;

	// These are no-ops for racing camera as it doesn't respond to direct input
	void OnMouseMove(double xpos, double ypos) override {}
	void OnMouseScroll(double xoffset, double yoffset) override {}
	void OnMouseButton(int button, int action, int mods) override {}

private:
	void UpdateIdealCameraPosition();

	RacingSettings m_racingSettings;
	glm::vec3 m_targetPosition{ 0.0f };
	glm::vec3 m_targetForward{ 0.0f, 0.0f, 1.0f };
	glm::vec3 m_targetUp{ 0.0f, 1.0f, 0.0f };
	glm::vec3 m_targetVelocity{ 0.0f };

	glm::vec3 m_currentPosition{ 0.0f };
	glm::vec3 m_idealPosition{ 0.0f };
	bool m_firstUpdate{ true };
};