#pragma once

#include <optional>

#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Text.hpp>

#include "Window.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "Clock.h"
#include "Lib.h"
#include "VectorLib.h"

class Camera
{
public:
    Camera();
    ~Camera();

    static void Update();
    static void Draw(const sf::Drawable &drawable, sf::RenderStates renderStates = sf::RenderStates::Default) noexcept;
    static void Draw(const sf::Drawable &drawable, sf::RenderTexture &texture, sf::RenderStates renderStates = sf::RenderStates::Default) noexcept;
    static void DrawText(const sf::Text &text, TextAlign align, sf::RenderStates renderStates = sf::RenderStates::Default) noexcept;
    static void DrawPoint(const sf::Vector2f &position, sf::Color color = sf::Color::Red, float radius = 3.0f) noexcept;
    static void DrawRect(const sf::FloatRect rect, sf::Color fillColor = sf::Color::Red, bool outlined = false, sf::Color outlineColor = sf::Color::Black);
    static void DrawLine(const sf::Vector2f &first, const sf::Vector2f &second, sf::Color color = sf::Color::Red);

    static void Move(const sf::Vector2f &offset) noexcept;
    static void Zoom(float factor) noexcept;
    static void Rotate(float angle) noexcept;

    static void SetCenter(const sf::Vector2f &center) noexcept;
    static void SetZoom(float zoom) noexcept;
    static void SetRotation(float angle) noexcept;

    static void Follow(const sf::Vector2f *follow) noexcept { m_follow = follow; }
    static void Unfollow() noexcept { m_follow.reset(); }

    ///Translate a point to world space
    ///@param point: point to be translated from screen to world space.
    static sf::Vector2f ScreenToWorld(const sf::Vector2f &point) noexcept;
    ///Translate a rect to world space
    ///@param rect: rect to be translated from screen to world space.
    static sf::FloatRect ScreenToWorld(const sf::FloatRect &rect) noexcept;
    ///Translate a point to world space
    ///@param rect: rect to be translated from world to screen space.
    static sf::Vector2f WorldToScreen(const sf::Vector2f &point) noexcept;
    ///Translate a rect to world space
    ///@param rect: rect to be translated from world to screen space.
    static sf::FloatRect WorldToScreen(const sf::FloatRect &rect) noexcept;

    static sf::FloatRect GetViewPort() noexcept { return m_transform.transformRect(Lib::ConvertTo<float>(Window::GetScreenRect())); }
    static sf::Vector2f GetViewSize() noexcept { return m_transform.transformPoint(GetViewPort().width, GetViewPort().height); }
    static sf::Vector2f GetOffset() noexcept { return Lib::ConvertTo<float>(Window::GetSize()) / 2.0f; }
    static float GetZoom() noexcept { return m_zoom.x; }

    static void SetRotationSpeed(float rps) noexcept { m_rps = rps; }

private:
    static void UpdateTransform() noexcept;
    static void CapZoomLevel() noexcept;
    static void ResetTransformation() noexcept;

private:
    static sf::Transform m_transform;
    static std::optional<const sf::Vector2f *> m_follow;

    static sf::Vector2f m_position;
    static sf::Transform m_positionTransform;

    static float m_rotation;
    static sf::Transform m_rotationTransform;
    static float m_rps; // Rotations per second

    static sf::Vector2f m_zoom;
    static sf::Transform m_zoomTransform;

    static bool m_engaged;
    static sf::Vector2f m_lastPos;
};