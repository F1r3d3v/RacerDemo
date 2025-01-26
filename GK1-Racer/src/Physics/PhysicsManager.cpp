#include "Physics/PhysicsManager.h"

PhysicsManager::PhysicsManager() {
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
	m_overlappingPairCache = new btDbvtBroadphase();
	m_solver = new btSequentialImpulseConstraintSolver;
	m_dynamicsWorld = new btDiscreteDynamicsWorld(
		m_dispatcher, m_overlappingPairCache, m_solver, m_collisionConfiguration
	);
	m_dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));
}

PhysicsManager::~PhysicsManager() {
	delete m_dynamicsWorld;
	delete m_solver;
	delete m_overlappingPairCache;
	delete m_dispatcher;
	delete m_collisionConfiguration;
}

void PhysicsManager::Update(float deltaTime) {
	m_dynamicsWorld->stepSimulation(deltaTime, 10);
}

void PhysicsManager::AddRigidBody(btRigidBody *body) {
	m_dynamicsWorld->addRigidBody(body);
}

void PhysicsManager::RemoveRigidBody(btRigidBody *body) {
	m_dynamicsWorld->removeRigidBody(body);
}