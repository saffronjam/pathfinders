#pragma once

#include <vector>

#include <Box2D/Dynamics/b2World.h>
#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Vector2.hpp>

#include "Collider.h"
#include "Clock.h"
#include "VectorLib.h"

class PhysicsMgr : public sf::NonCopyable
{
public:
    PhysicsMgr();

    static void Step();

    static void AddCollider(Collider *collider, const b2BodyDef *bodyDef);
    static b2World *GetWorld() noexcept { return m_world.get(); }

private:
    static std::unique_ptr<b2World> m_world;
    static std::vector<Collider::Ptr> m_colliders;
};