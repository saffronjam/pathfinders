#include "Light.h"

void Light::BuildBase(const sf::Vector2f &position, float radius, sf::Color color, const sf::Vector2f &direction, float theta)
{
    m_position = position;
    m_radius = radius;
    m_color = color;
    m_staticOccluderMap.create(GetBoundingBox().width, GetBoundingBox().height);
    m_staticOccluderMap.clear(sf::Color(0, 0, 0, 0));
}

void Light::AddStaticOccluder(const sf::Drawable &drawable, const sf::RenderStates &renderStates) noexcept
{
    m_staticOccluderMap.draw(drawable, renderStates);
    m_staticOccluderMap.display();
}