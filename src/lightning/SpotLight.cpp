#include "SpotLight.h"

SpotLight::Ptr SpotLight::Create(const sf::Vector2f &position, const sf::Vector2f &direction, float theta, float radius, sf::Color color)
{
    Ptr ret = std::make_shared<SpotLight>();
    ret->BuildBase(position, radius, color, direction, theta);
    ret->SetDirection(direction, theta);
    return ret;
}

void SpotLight::SetDirection(const sf::Vector2f &newDirection, float theta)
{
    ClearStaticOccluders();

    sf::Vector2f unitDirection = vl::Unit(newDirection);
    sf::VertexArray shape(sf::TriangleFan);
    sf::Vector2f adjustedPosition = m_position + sf::Vector2f(m_radius, m_radius);
    shape.append(sf::Vertex(adjustedPosition - unitDirection * 10.0f, sf::Color::Black));
    shape.append(sf::Vertex(adjustedPosition + vl::Rotate(unitDirection, Lib::ToRadians(theta / 2.0f), vl::Null<>()) * 50.0f, sf::Color::Black));
    shape.append(sf::Vertex(adjustedPosition - unitDirection * 25.0f, sf::Color::Black));
    shape.append(sf::Vertex(adjustedPosition + vl::Rotate(unitDirection, Lib::ToRadians(-theta / 2.0f), vl::Null<>()) * 50.0f, sf::Color::Black));

    AddStaticOccluder(shape);
}