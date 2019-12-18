#pragma once

#include "Graphics.hpp"

class Camera
{
public:
    Camera(Graphics &gfx, sf::Vector2f &to_follow, class CameraController &camera_controller);

    void Update(sf::Time dt);

    void PushChain(const sf::Drawable &drawable);

    void SetZoom(sf::Vector2f zoom) { m_zoom = zoom; }
    void SetAngle(float angle) { m_angle = angle; }

    sf::Vector2f GetPos() const { return m_pos; }
    sf::Vector2f GetZoom() const { return m_zoom; }
    float GetAngle() const { return m_angle; }
    sf::Rect<float> GetViewportRect() const;

    void MoveBy(const sf::Vector2f &offset) { m_pos += offset; }
    void MoveTo(const sf::Vector2f &pos_in) { m_pos = pos_in; }

private:
    void CapZoomLevel();

private:
    Graphics &m_gfx;
    sf::Vector2f m_pos;

    sf::Vector2f m_zoom;
    float m_angle;

    sf::Vector2f &m_to_follow;
    class CameraController &m_camera_controller;

public:
    static constexpr int OffsetX = Graphics::ScreenWidth / 2;
    static constexpr int OffsetY = Graphics::ScreenHeight / 2;
    static sf::Vector2i GetOffset() { return sf::Vector2i(OffsetX, OffsetY); };
};