#include "InputUtility.hpp"

#include "Math.hpp"

#include <sstream>

using namespace sf;

InputUtility::InputUtility(Graphics &gfx, Camera &camera)
    : m_gfx(gfx),
      m_camera(camera)
{
    m_mouseCoordinates.setFont(*m_gfx.allFonts[Graphics::FontMapping::FontArial]);
    m_mouseCoordinates.setCharacterSize(8);
    for (int i = 0; i < sf::Keyboard::Key::KeyCount; i++)
    {
        m_keys_lastState[i] = false;
        m_keys_wasReleased[i] = true;
    }
}

void InputUtility::Update()
{
    for (int i = 0; i < sf::Keyboard::Key::KeyCount; i++)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key(i)) && m_keys_wasReleased[i])
        {
            m_keys_wasReleased[i] = false;
            m_keys_lastState[i] ? m_keys_lastState[i] = false : m_keys_lastState[i] = true;
        }
        else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key(i)))
        {
            m_keys_wasReleased[i] = true;
        }
    }

    for (int i = 0; i < sf::Mouse::Button::ButtonCount; i++)
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button(i)) && m_buttons_wasReleased[i])
        {
            m_buttons_wasReleased[i] = false;
            m_buttons_lastState[i] ? m_buttons_lastState[i] = false : m_buttons_lastState[i] = true;
        }
        else if (sf::Mouse::isButtonPressed(sf::Mouse::Button(i)))
        {
            m_buttons_wasReleased[i] = true;
        }
    }
}

void InputUtility::DrawMouseCoordinates()
{
    sf::Vector2i posCamera = GetMousePosition();
    sf::Vector2i posWindow = sf::Mouse::getPosition(m_gfx.GetRenderWindow());
    sf::Vector2i posScreen = sf::Mouse::getPosition();
    std::stringstream stream;
    stream << "C: ( " << posCamera.x << " | " << posCamera.y << " )" << std::endl
           << "W: ( " << posWindow.x << " | " << posWindow.y << " )" << std::endl
           << "S: ( " << posScreen.x << " | " << posScreen.y << " )";
    std::string coords = stream.str();
    m_mouseCoordinates.setString(coords);
    m_mouseCoordinates.setPosition(sf::Vector2f(Graphics::ScreenWidth - m_mouseCoordinates.getLocalBounds().width - 3, 3));
    m_gfx.Render(m_mouseCoordinates);
}

bool InputUtility::IsToggled(const sf::Keyboard::Key key)
{
    return m_keys_lastState[key];
}

bool InputUtility::IsToggled(const sf::Mouse::Button button)
{
    return m_buttons_lastState[button];
}

sf::Vector2i InputUtility::GetMousePosition()
{
    sf::Vector2i final = sf::Mouse::getPosition(m_gfx.GetRenderWindow());
    final = (sf::Vector2i)m_gfx.AddDefaultTranslation((sf::Vector2f)final);
    final = (sf::Vector2i)m_gfx.AddDefaultRotation((sf::Vector2f)final);
    final = (sf::Vector2i)m_gfx.AddDefaultScale((sf::Vector2f)final);
    return final;
}
