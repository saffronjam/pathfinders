#pragma once

#include <utility>
#include <memory>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

#include "Lib.h"
#include "VectorLib.h"

class Light
{
    friend class LightningMgr;

public:
    using Ptr = std::shared_ptr<Light>;

protected:
    void BuildBase(const sf::Vector2f &position, float radius, sf::Color color = sf::Color::White, const sf::Vector2f &direction = vl::Null<>(), float theta = 0.0f);

public:
    const sf::Vector2f &GetPosition() const noexcept { return m_position; }
    float GetRadius() const noexcept { return m_radius; }
    const sf::Color &GetColor() const noexcept { return m_color; }
    sf::FloatRect GetBoundingBox() const noexcept { return sf::FloatRect(m_position.x - GetRadius(), m_position.y - GetRadius(), GetRadius() * 2.0f, GetRadius() * 2.0f); }

    void SetPosition(const sf::Vector2f &position) noexcept { m_position = position; }
    void SetRadius(float radius) noexcept { m_radius = radius; }
    void SetColor(const sf::Color &color) noexcept { m_color = color; }

protected:
    void AddStaticOccluder(const sf::Drawable &drawable, const sf::RenderStates &renderStates = sf::RenderStates::Default) noexcept;
    void ClearStaticOccluders() noexcept { m_staticOccluderMap.clear(sf::Color(0, 0, 0, 0)); }
    const sf::RenderTexture &GetStaticOccluderMap() const noexcept { return m_staticOccluderMap; }

protected:
    sf::Vector2f m_position;
    float m_radius;
    sf::Color m_color;
    sf::RenderTexture m_staticOccluderMap;
};