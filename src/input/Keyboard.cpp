#include "Keyboard.h"

std::map<sf::Keyboard::Key, bool> Keyboard::m_keymap;
std::map<sf::Keyboard::Key, bool> Keyboard::m_prevKeymap;
std::map<Keyboard::CallbackEvent, std::vector<Keyboard::Callback>> Keyboard::m_callbacks;
std::string Keyboard::m_textInputBuffer;

Keyboard::Keyboard() noexcept
{
    EventMgr::AddHandler(this);
}

Keyboard::~Keyboard() noexcept
{
}

void Keyboard::Update() noexcept
{
    for (auto &[key, state] : m_keymap)
        m_prevKeymap[key] = state;
    m_textInputBuffer.clear();
}

bool Keyboard::IsDown(const sf::Keyboard::Key &key) noexcept
{
    return m_keymap[key];
}

bool Keyboard::IsPressed(const sf::Keyboard::Key &key) noexcept
{
    return m_keymap[key] && !m_prevKeymap[key];
}

bool Keyboard::IsReleased(const sf::Keyboard::Key &key) noexcept
{
    return !m_keymap[key] && m_prevKeymap[key];
}

bool Keyboard::IsAnyDown() noexcept
{
    for (auto &[key, state] : m_keymap)
    {
        if (m_keymap[key])
            return true;
    }
    return false;
}

void Keyboard::OnEvent(const sf::Event &event) noexcept
{
    switch (event.type)
    {
    case sf::Event::EventType::KeyPressed:
    {
        OnPress(event.key);
        break;
    }
    case sf::Event::EventType::KeyReleased:
    {
        OnRelease(event.key);
        break;
    }
    case sf::Event::EventType::TextEntered:
    {
        OnTextInput(event.text.unicode);
        break;
    }
    }
}

void Keyboard::OnPress(const sf::Event::KeyEvent &event) noexcept
{
    auto &vector = m_callbacks[CallbackEvent::KeyPressed];
    for (auto &callback : vector)
        callback(event);
    m_keymap[event.code] = true;
}

void Keyboard::OnRelease(const sf::Event::KeyEvent &event) noexcept
{
    auto &vector = m_callbacks[CallbackEvent::KeyReleased];
    for (auto &callback : vector)
        callback(event);
    m_keymap[event.code] = false;
}

void Keyboard::OnTextInput(unsigned char character) noexcept
{
    m_textInputBuffer += character;
}