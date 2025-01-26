#pragma once
#include "Engine/Objects/GraphicsObject.h"
#include "Physics/VehicleController.h"
#include "Engine/Objects/Model.h"

class Vehicle : public GraphicsObject {
public:
	Vehicle(btDynamicsWorld *world,
		std::shared_ptr<Model> vehicleMesh,
		const glm::vec3 &startPosition);

	void Update(float deltaTime);
	void Draw() override;

	std::shared_ptr<VehicleController> GetController() { return m_controller; }
	std::shared_ptr<Model> GetModel() { return m_model; }

private:
	std::shared_ptr<VehicleController> m_controller;
	std::shared_ptr<Model> m_model;
};