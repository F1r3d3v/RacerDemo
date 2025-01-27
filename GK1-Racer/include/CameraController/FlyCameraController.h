#pragma once
#include "CameraController/BaseCameraController.h"

class FlyCameraController : public BaseCameraController {

public:
	struct FlySettings : public Settings {
		float moveSpeed{ 5.0f };
	};

	FlyCameraController(std::shared_ptr<Camera> camera, const FlySettings &settings = FlySettings{});
	~FlyCameraController() = default;

	void Update(float deltaTime) override;

protected:
	float m_moveSpeed;
};