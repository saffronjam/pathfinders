#pragma once

#include "Light.h"

class PointLight : public Light
{
public:
    using Ptr = std::shared_ptr<PointLight>;

public:
    static PointLight::Ptr Create(const sf::Vector2f &position, float radius, sf::Color color = sf::Color::White);
};