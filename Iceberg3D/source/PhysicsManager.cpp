#include "PhysicsManager.h"
#include "Game.h"

using namespace iceberg;

PhysicsManager::PhysicsManager()
{
    // Initialize Bullet
    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

    // Add gravity
    dynamicsWorld->setGravity(btVector3(0.0f, -9.81f, 0.0f));
}

PhysicsManager::~PhysicsManager()
{
    // Free Bullet
    for (int i = 0; i < dynamicsWorld->getNumCollisionObjects(); i++)
    {
        dynamicsWorld->removeRigidBody(btRigidBody::upcast(dynamicsWorld->getCollisionObjectArray()[i]));
    }

    delete dynamicsWorld;
    delete broadphase;
    delete collisionConfiguration;
    delete dispatcher;
    delete solver;

    dynamicsWorld = nullptr;
    broadphase = nullptr;
    collisionConfiguration = nullptr;
    dispatcher = nullptr;
    solver = nullptr;
}

void PhysicsManager::AddRigidBody(btCollisionShape* collisionShape, btVector3* origin, btScalar* mass, btScalar* restitution, btVector3* inertia, bool kinematic) const
{
    btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), *origin));
    if (mass > 0)
        collisionShape->calculateLocalInertia(*mass, *inertia);
    btRigidBody::btRigidBodyConstructionInfo constructionInfo(*mass, motionState, collisionShape, *inertia);
    btRigidBody* body = new btRigidBody(constructionInfo);
    body->setRestitution(*restitution);
    body->setActivationState(DISABLE_DEACTIVATION);
    if (kinematic)
    {
        body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
        body->setActivationState(DISABLE_DEACTIVATION);
    }
    dynamicsWorld->addRigidBody(body);
}

int PhysicsManager::GetNumObjects() const
{
    return dynamicsWorld->getNumCollisionObjects();
}

glm::mat4 PhysicsManager::GetModelMatrixAtIndex(int index) const
{
    if (index < 0 || index >= GetNumObjects())
    {
        std::cout << "ERROR! PhysicsManager::GetModelMatrixAtIndex tried to access: " << index << " which is not between 0 and " << GetNumObjects() << "." << std::endl;
        return glm::mat4(1.0f);
    }

    btTransform trans;
    btScalar m[16];
    btRigidBody* object = btRigidBody::upcast(dynamicsWorld->getCollisionObjectArray()[index]);
    object->getMotionState()->getWorldTransform(trans);
    trans.getOpenGLMatrix(m);
    glm::mat4 modelMatrix = glm::make_mat4(m);
    return modelMatrix;
}

void PhysicsManager::Update(Game* game) const
{
    dynamicsWorld->stepSimulation(game->delta_time(), 1);
}

void PhysicsManager::ApplyForceAtIndex(btVector3* force, int index) const
{
    if (index < 0 || index >= GetNumObjects())
    {
        std::cout << "ERROR! PhysicsManager::ApplyForceAtIndex tried to access: " << index << " which is not between 0 and " << GetNumObjects() << "." << std::endl;
        return;
    }

    btRigidBody* object = btRigidBody::upcast(dynamicsWorld->getCollisionObjectArray()[index]);
    object->applyCentralImpulse(*force);
    object->clearForces();
}