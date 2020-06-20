#include "Mouse.h"

std::map<sf::Mouse::Button, bool> Mouse::m_buttonmap;
std::map<sf::Mouse::Button, bool> Mouse::m_prevButtonmap;
std::map<Mouse::CallbackEvent, std::vector<Mouse::ButtonCallback>> Mouse::m_buttonCallbacks;
std::map<Mouse::CallbackEvent, std::vector<Mouse::ScrollCallback>> Mouse::m_scrollCallbacks;
std::map<Mouse::CallbackEvent, std::vector<Mouse::MoveCallback>> Mouse::m_moveCallbacks;
std::map<Mouse::CallbackEvent, std::vector<Mouse::EnterLeaveCallback>> Mouse::m_enterLeaveCallbacks;
sf::Vector2f Mouse::m_mousePos = sf::Vector2f(0.0f, 0.0f);
bool Mouse::m_inWindow = true;
float Mouse::m_verticalScrollBuffer = 0.0f;
float Mouse::m_horizontalScrollBuffer = 0.0f;

Mouse::Mouse() noexcept
{
    EventMgr::AddHandler(this);
}

Mouse::~Mouse() noexcept
{
}

void Mouse::Update() noexcept
{
    for (auto &[key, state] : m_buttonmap)
        m_prevButtonmap[key] = state;

    m_horizontalScrollBuffer = 0.0f;
    m_verticalScrollBuffer = 0.0f;
}

bool Mouse::IsDown(const sf::Mouse::Button &button) noexcept
{
    return m_buttonmap[button];
}

bool Mouse::IsPressed(const sf::Mouse::Button &button) noexcept
{
    return m_buttonmap[button] && !m_prevButtonmap[button];
}

bool Mouse::IsReleased(const sf::Mouse::Button &button) noexcept
{
    return !m_buttonmap[button] && m_prevButtonmap[button];
}

bool Mouse::AnyButtonDown() noexcept
{
    for (auto &[button, state] : m_buttonmap)
        if (state)
            return true;
    return false;
}

void Mouse::OnEvent(const sf::Event &event) noexcept
{
    switch (event.type)
    {
    case sf::Event::EventType::MouseButtonPressed:
        OnPress(event.mouseButton);
        break;

    case sf::Event::EventType::MouseButtonReleased:
        OnRelease(event.mouseButton);
        break;

    case sf::Event::EventType::MouseMoved:
        OnMove(event.mouseMove);
        break;

    case sf::Event::EventType::MouseEntered:
        OnEnter();
        break;

    case sf::Event::EventType::MouseLeft:
        OnLeave();
        break;

    case sf::Event::EventType::MouseWheelScrolled:
        OnScroll(event.mouseWheelScroll);
        break;
    }
}

void Mouse::OnPress(const sf::Event::MouseButtonEvent &event) noexcept
{
    auto &vector = m_buttonCallbacks[CallbackEvent::MouseButtonPressed];
    for (auto &callback : vector)
        callback(event);
    m_buttonmap[event.button] = true;
}

void Mouse::OnRelease(const sf::Event::MouseButtonEvent &event) noexcept
{
    auto &vector = m_buttonCallbacks[CallbackEvent::MouseButtonReleased];
    for (auto &callback : vector)
        callback(event);
    m_buttonmap[event.button] = false;
}

void Mouse::OnMove(const sf::Event::MouseMoveEvent &event) noexcept
{
    auto &vector = m_moveCallbacks[CallbackEvent::MouseMoved];
    for (auto &callback : vector)
        callback(event);
    if (!m_inWindow && AnyButtonDown() || m_inWindow)
    {
        m_mousePos = sf::Vector2f(event.x, event.y);
    }
}

void Mouse::OnEnter() noexcept
{
    auto &vector = m_enterLeaveCallbacks[CallbackEvent::MouseEntered];
    for (auto &callback : vector)
        callback();
    m_inWindow = true;
}

void Mouse::OnLeave() noexcept
{
    auto &vector = m_enterLeaveCallbacks[CallbackEvent::MouseLeft];
    for (auto &callback : vector)
        callback();
    m_inWindow = false;
}

void Mouse::OnScroll(const sf::Event::MouseWheelScrollEvent &event) noexcept
{
    auto &vector = m_scrollCallbacks[CallbackEvent::MouseWheelScrolled];
    for (auto &callback : vector)
        callback(event);
    if (event.wheel == sf::Mouse::Wheel::HorizontalWheel)
    {
        m_horizontalScrollBuffer += event.delta;
    }
    else if (event.wheel == sf::Mouse::Wheel::VerticalWheel)
    {
        m_verticalScrollBuffer += event.delta;
    }
}
