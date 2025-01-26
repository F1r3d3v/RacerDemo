#include "Objects/Vehicle.h"

Vehicle::Vehicle(btDynamicsWorld *world,
	std::shared_ptr<Model> vehicleMesh,
	const glm::vec3 &startPosition)
	: m_model(vehicleMesh) {
	m_controller = std::make_shared<VehicleController>(world, startPosition);
}

void Vehicle::Update(float deltaTime) {
	m_controller->Update(deltaTime);

	// Update mesh to match physics simulation
	SetPosition(m_controller->GetPosition());
	SetRotation(m_controller->GetRotation());
}

void Vehicle::Draw() {
	if (m_model) {
		m_model->ApplyTransformations(GetWorldMatrix());
		m_model->Draw();
	}
}