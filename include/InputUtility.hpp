#pragma once

#include <array>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include "Camera.hpp"

class InputUtility
{
public:
    InputUtility(Graphics &gfx, Camera &camera);

    void Update();

    void DrawMouseCoordinates();

    bool IsToggled(const sf::Keyboard::Key key);
    bool IsToggled(const sf::Mouse::Button button);

    sf::Vector2i GetMousePosition();

private:
    Graphics &m_gfx;
    Camera &m_camera;

    sf::Text m_mouseCoordinates;

    std::array<bool, sf::Keyboard::Key::KeyCount> m_keys_lastState;
    std::array<bool, sf::Keyboard::Key::KeyCount> m_keys_wasReleased;
    std::array<bool, sf::Mouse::Button::ButtonCount> m_buttons_lastState;
    std::array<bool, sf::Mouse::Button::ButtonCount> m_buttons_wasReleased;
};