#include "PhysicsMgr.h"

std::unique_ptr<b2World> PhysicsMgr::m_world;

PhysicsMgr::PhysicsMgr()
{
    m_world = std::make_unique<b2World>(b2Vec2(0.0f, 0.0f));
}

void PhysicsMgr::Step()
{
    m_world->Step(1.0f / 60.0f, 6, 2);
}

void PhysicsMgr::AddCollider(Collider *collider, const b2BodyDef *bodyDef)
{
    collider->AssignBody(m_world->CreateBody(bodyDef));
}
