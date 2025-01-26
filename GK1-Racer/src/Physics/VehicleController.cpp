#include "Physics/VehicleController.h"
#include <glm/gtc/quaternion.hpp>

VehicleController::VehicleController(btDynamicsWorld *world, const glm::vec3 &startPosition, const VehicleParameters &params)
	: m_params(params) {
	// Chassis shape
	btCollisionShape *chassisShape = new btBoxShape(btVector3(1.0f, 0.5f, 2.0f));

	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(startPosition.x, startPosition.y, startPosition.z));

	btScalar mass(m_params.mass);
	btVector3 localInertia(0, 0, 0);
	chassisShape->calculateLocalInertia(mass, localInertia);

	// Create rigid body
	btDefaultMotionState *motionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, chassisShape, localInertia);
	m_chassisBody = std::unique_ptr<btRigidBody>(new btRigidBody(rbInfo));

	// Vehicle raycaster and vehicle
	m_raycaster = std::unique_ptr<btVehicleRaycaster>(new btDefaultVehicleRaycaster(world));
	btRaycastVehicle::btVehicleTuning tuning;
	m_vehicle = std::unique_ptr<btRaycastVehicle>(new btRaycastVehicle(tuning, m_chassisBody.get(), m_raycaster.get()));
	m_vehicle->setCoordinateSystem(2, 1, 0);

	// Configure wheels
	btVector3 wheelDirectionCS0(0, -1, 0);
	btVector3 wheelAxleCS(-1, 0, 0);

	// Add wheels
	bool isFrontWheel = true;
	m_vehicle->addWheel(btVector3(1.0f, 0, 1.5f), wheelDirectionCS0, wheelAxleCS, m_params.suspensionRestLength, m_params.wheelRadius, tuning, isFrontWheel);
	m_vehicle->addWheel(btVector3(-1.0f, 0, 1.5f), wheelDirectionCS0, wheelAxleCS, m_params.suspensionRestLength, m_params.wheelRadius, tuning, isFrontWheel);

	isFrontWheel = false;
	m_vehicle->addWheel(btVector3(1.0f, 0, -1.5f), wheelDirectionCS0, wheelAxleCS, m_params.suspensionRestLength, m_params.wheelRadius, tuning, isFrontWheel);
	m_vehicle->addWheel(btVector3(-1.0f, 0, -1.5f), wheelDirectionCS0, wheelAxleCS, m_params.suspensionRestLength, m_params.wheelRadius, tuning, isFrontWheel);

	// Tune wheels
	for (int i = 0; i < m_vehicle->getNumWheels(); i++) {
		btWheelInfo &wheel = m_vehicle->getWheelInfo(i);
		wheel.m_suspensionStiffness = m_params.suspensionStiffness;
		wheel.m_wheelsDampingRelaxation = m_params.suspensionDamping;
		wheel.m_wheelsDampingCompression = m_params.suspensionCompression;
		wheel.m_frictionSlip = m_params.friction;
		wheel.m_rollInfluence = m_params.rollInfluence;
	}

	world->addAction(m_vehicle.get());
	world->addRigidBody(m_chassisBody.get());
}

void VehicleController::Update(float deltaTime) {
	m_vehicle->updateVehicle(deltaTime);

	// Sync transform with physics world
	btTransform transform = m_vehicle->getChassisWorldTransform();
	btVector3 origin = transform.getOrigin();
	m_position = glm::vec3(origin.x(), origin.y(), origin.z());

	btQuaternion orientation = transform.getRotation();
	orientation *= btQuaternion(btVector3(0, 1, 0), SIMD_PI);
	btMatrix3x3 rotationMatrix(orientation);

	btVector3 forward = -rotationMatrix.getColumn(2);
	btVector3 right = rotationMatrix.getColumn(0);
	btVector3 up = rotationMatrix.getColumn(1);

	// Update rotation
	m_rotation = Transform::GetRotation(glm::inverse(glm::mat3(
		right.x(), right.y(), right.z(),
		up.x(), up.y(), up.z(),
		forward.x(), forward.y(), forward.z()
	)));

	// Update wheels
	//for (int i = 0; i < m_vehicle->getNumWheels(); i++) {
	//	m_vehicle->updateWheelTransform(i, true);
	//	btTransform wheelTransform = m_vehicle->getWheelTransformWS(i);
	//	btVector3 wheelOrigin = wheelTransform.getOrigin();
	//	btQuaternion wheelRotation = wheelTransform.getRotation();
	//	// Update wheel transforms
	//	glm::vec3 wheelPosition(wheelOrigin.x(), wheelOrigin.y(), wheelOrigin.z());
	//	glm::quat wheelRotationQuat(wheelRotation.w(), wheelRotation.x(), wheelRotation.y(), wheelRotation.z());
	//	// TODO: Update wheel transforms
	//}
}

void VehicleController::ApplyEngineForce(float force) {
	for (int i = 0; i < 4; ++i) {
		m_vehicle->applyEngineForce(force, i);
	}
}

void VehicleController::Steer(float angle) {
	m_vehicle->setSteeringValue(angle, 0);
	m_vehicle->setSteeringValue(angle, 1);
}

void VehicleController::Brake(float force) {
	for (int i = 0; i < 4; ++i) {
		m_vehicle->setBrake(force, i);
	}
}

glm::vec3 VehicleController::GetVehicleVelocity() const {
	btVector3 velocity = m_chassisBody->getLinearVelocity();
	return glm::vec3(velocity.x(), velocity.y(), velocity.z());
}

void VehicleController::Flip()
{
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(m_position.x, m_position.y + 2.0f, m_position.z));
	m_chassisBody->setWorldTransform(transform);
	m_chassisBody->setLinearVelocity(btVector3(0, 0, 0));
	m_chassisBody->setAngularVelocity(btVector3(0, 0, 0));
	m_chassisBody->activate(true);
}
