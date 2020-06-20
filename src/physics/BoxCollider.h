#pragma once

#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

#include "PhysicsMgr.h"

class BoxCollider : public Collider
{
public:
    using Ptr = std::shared_ptr<BoxCollider>;

public:
    static Ptr Create(const sf::Vector2f &position, const sf::Vector2f &dimensions);

    const sf::Vector2f &GetDimensions() const noexcept { return m_dimensions; }
    sf::FloatRect GetRect() const;

private:
    sf::Vector2f m_dimensions;
};