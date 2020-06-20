#pragma once

#include <Box2D/Collision/Shapes/b2CircleShape.h>
#include <SFML/System/Vector2.hpp>

#include "PhysicsMgr.h"

class CircleCollider : public Collider
{
public:
    using Ptr = std::shared_ptr<CircleCollider>;

public:
    static Ptr Create(const sf::Vector2f &position, float radius);

    float GetRadius() const noexcept { return m_radius; }

private:
    float m_radius;
};