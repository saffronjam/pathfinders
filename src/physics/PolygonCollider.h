#pragma once

#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include <SFML/System/Vector2.hpp>

#include "PhysicsMgr.h"

class PolygonCollider : public Collider
{
public:
    using Ptr = std::shared_ptr<PolygonCollider>;

public:
    static Ptr Create(const sf::Vector2f &position, std::vector<sf::Vector2f> points);

private:
};