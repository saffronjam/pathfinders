#include "Collider.h"

#include "PhysicsMgr.h"

void Collider::BuildBase(b2BodyDef *bodyDef, b2FixtureDef *fixtureDef)
{
    PhysicsMgr::AddCollider(this, bodyDef);
    m_fixture = m_body.value()->CreateFixture(fixtureDef);
}