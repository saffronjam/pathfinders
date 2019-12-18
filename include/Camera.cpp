#include "Camera.hpp"

#include "CameraController.hpp"

#include "Math.hpp"

Camera::Camera(Graphics &gfx, sf::Vector2f &to_follow, CameraController &camera_controller)
    : m_gfx(gfx),
      m_pos(0.0f, 0.0f),
      m_zoom(1.0f, 1.0f),
      m_angle(0.0f),
      m_to_follow(to_follow),
      m_camera_controller(camera_controller)
{
}

void Camera::Update(sf::Time dt)
{
    CapZoomLevel();
    if (!m_camera_controller.GetEngaged())
    {
        m_pos = m_to_follow;
    }
}

void Camera::PushChain(const sf::Drawable &drawable)
{
    const sf::Vector2f offset(float(Graphics::ScreenWidth / 2), float(Graphics::ScreenHeight / 2));
    sf::Transform t = sf::Transform::Identity;
    t.translate(offset);
    t.rotate(m_angle);
    t.scale(m_zoom);
    t.translate(-m_pos);
    m_gfx.ApplyTransformation(t);
    m_gfx.Render(drawable);
}

sf::Rect<float> Camera::GetViewportRect() const
{
    const sf::Vector2f zoom = sf::Vector2f(1.0f / m_zoom.x, 1.0f / m_zoom.y);
    const float diagonal = sqrt(
        pow(float(Graphics::ScreenWidth / 2) * zoom.x, 2) +
        pow(float(Graphics::ScreenHeight / 2) * zoom.y, 2));
    return sfmlext::RectFromCenter(m_pos, diagonal, diagonal);
}

void Camera::CapZoomLevel()
{
    gf::Constrain(m_zoom.x, 0.9f, 3.0f);
    gf::Constrain(m_zoom.y, 0.9f, 3.0f);
}