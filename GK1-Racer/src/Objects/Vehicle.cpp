#include "Objects/Vehicle.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

Vehicle::Vehicle(btDynamicsWorld *world,
	std::shared_ptr<Model> vehicleMesh,
	std::shared_ptr<Model> wheelMesh,
	const glm::vec3 &startPosition)
	: m_Body(vehicleMesh), m_Wheel(wheelMesh) {
	m_controller = std::make_shared<VehicleController>(world, startPosition);
}

void Vehicle::Update(float deltaTime) {
	m_controller->Update(deltaTime);

	// Update mesh to match physics simulation
	SetPosition(m_controller->GetPosition());
	SetOrientation(m_controller->GetOrientation());
}

void Vehicle::Draw() {
	if (m_Body) {
		auto wheels = m_controller->GetWheels();	
		auto params = m_controller->GetParameters();
		int i = 0;
		for (const auto &wheel : wheels) {
			// Get Up vector
			btVector3 down = -m_controller->GetBulletVehicle()->getChassisWorldTransform().getBasis().getColumn(1);
			glm::vec3 pos = wheel.position + (params.wheelRadius/2) * glm::vec3(down.x(), down.y(), down.z());
			glm::quat orientation = glm::rotate(wheel.orientation, glm::radians((i == 1 || i == 3) ? 180.0f : 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * glm::toMat4(orientation);
			m_Wheel->ApplyTransformations(transform);
			m_Wheel->Draw();
			i++;
		}
		m_Body->ApplyTransformations(GetWorldMatrix());
		m_Body->Draw();
	}
}