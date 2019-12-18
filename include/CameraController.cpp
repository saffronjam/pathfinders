#include "CameraController.hpp"

#include "Math.hpp"
#include "InputUtility.hpp"

CameraController::CameraController(Graphics &gfx, Camera &camera, InputUtility &iu)
    : m_gfx(gfx),
      m_camera(camera),
      m_iu(iu),
      m_lastPos(sf::Mouse::getPosition(m_gfx.GetRenderWindow())),
      m_rotationSpeed(30)
{
}

void CameraController::Update(sf::Time dt)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q))
    {
        m_camera.SetAngle(m_camera.GetAngle() + m_rotationSpeed * dt.asSeconds());
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E))
    {
        m_camera.SetAngle(m_camera.GetAngle() - m_rotationSpeed * dt.asSeconds());
    }

    const sf::Vector2i cur_pos = sf::Mouse::getPosition(m_gfx.GetRenderWindow());
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
    {
        m_engaged = true;
        sf::Vector2i delta = cur_pos - m_lastPos;
        if (vf::LengthSq((sf::Vector2f)delta) > 0.0f)
        {
            m_lastPos = cur_pos;
            delta = (sf::Vector2i)m_gfx.AddDefaultRotation((sf::Vector2f)delta);
            delta = (sf::Vector2i)m_gfx.AddDefaultScale((sf::Vector2f)delta);
            delta *= -1;
            m_camera.MoveBy((sf::Vector2f)delta);
        }
    }
    else
    {
        m_lastPos = cur_pos;
        m_engaged = false;
    }
}