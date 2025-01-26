#pragma once
#include "Engine/Transform.h"
#include <btBulletDynamicsCommon.h>
#include <memory>
#include <glm/glm.hpp>

class VehicleController {
public:
	struct VehicleParameters {
		float mass = 1000.0f;
		float wheelRadius = 0.5f;
		float suspensionRestLength = 0.6f;
		float suspensionStiffness = 20.0f;
		float suspensionDamping = 2.3f;
		float suspensionCompression = 4.4f;
		float friction = 10.0f;
		float rollInfluence = 0.1f;
	};

	VehicleController(btDynamicsWorld *world, const glm::vec3 &startPosition,
		const VehicleParameters &params = VehicleParameters{});
	~VehicleController() = default;

	void Update(float deltaTime);
	void ApplyEngineForce(float force);
	void Steer(float angle);
	void Brake(float force);

	glm::vec3 GetVehicleVelocity() const;
	glm::vec3 GetPosition() const { return m_position; }
	glm::vec3 GetRotation() const { return m_rotation; }
	btRaycastVehicle *GetBulletVehicle() { return m_vehicle.get(); }
	btRigidBody *GetChassisBody() { return m_chassisBody.get(); }
	void Flip();

private:
	std::unique_ptr<btRigidBody> m_chassisBody;
	std::unique_ptr<btRaycastVehicle> m_vehicle;
	std::unique_ptr<btVehicleRaycaster> m_raycaster;

	glm::vec3 m_position;
	glm::vec3 m_rotation;

	VehicleParameters m_params;
};