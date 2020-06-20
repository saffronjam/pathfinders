#pragma once

#include <map>

#include "EventMgr.h"

class Keyboard : public EventHandler
{
public:
    using Callback = std::function<void(const sf::Event::KeyEvent &)>;
    enum class CallbackEvent
    {
        KeyPressed,
        KeyReleased
    };

public:
    // Set up call-back function in eventMgr
    Keyboard() noexcept;
    ~Keyboard() noexcept;
    Keyboard(const Keyboard &) = delete;
    Keyboard &operator=(const Keyboard &) = delete;

    // Move keymap into prev-keymap
    static void Update() noexcept;

    template <typename T>
    static void AddCallback(const CallbackEvent &event, T callback) noexcept
    {
        switch (event)
        {
        case CallbackEvent::KeyPressed:
        case CallbackEvent::KeyReleased:
            m_callbacks[event].push_back(callback);
            break;
        default:
            break;
        }
    }

    static bool IsDown(const sf::Keyboard::Key &key) noexcept;
    static bool IsPressed(const sf::Keyboard::Key &key) noexcept;
    static bool IsReleased(const sf::Keyboard::Key &key) noexcept;
    static bool IsAnyDown() noexcept;
    static std::string GetTextInput() noexcept { return m_textInputBuffer; }

private:
    virtual void OnEvent(const sf::Event &event) noexcept override;

    static void OnPress(const sf::Event::KeyEvent &event) noexcept;
    static void OnRelease(const sf::Event::KeyEvent &event) noexcept;
    static void OnTextInput(unsigned char character) noexcept;

private:
    static std::map<sf::Keyboard::Key, bool> m_keymap;
    static std::map<sf::Keyboard::Key, bool> m_prevKeymap;
    static std::map<CallbackEvent, std::vector<Callback>> m_callbacks;
    static std::string m_textInputBuffer;
};