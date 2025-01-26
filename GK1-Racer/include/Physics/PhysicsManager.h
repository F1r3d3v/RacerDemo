#pragma once
#include <btBulletDynamicsCommon.h>
#include <memory>
#include <vector>

class PhysicsManager {
public:
	PhysicsManager();
	~PhysicsManager();

	void Update(float deltaTime);
	btDiscreteDynamicsWorld *GetDynamicsWorld() { return m_dynamicsWorld; }
	void AddRigidBody(btRigidBody *body);
	void RemoveRigidBody(btRigidBody *body);

private:
	btDefaultCollisionConfiguration *m_collisionConfiguration;
	btCollisionDispatcher *m_dispatcher;
	btBroadphaseInterface *m_overlappingPairCache;
	btSequentialImpulseConstraintSolver *m_solver;
	btDiscreteDynamicsWorld *m_dynamicsWorld;
};
