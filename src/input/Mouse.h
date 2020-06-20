#pragma once

#include <map>

#include <SFML/Window/Mouse.hpp>

#include "EventMgr.h"

class Mouse : public EventHandler
{
public:
    using ButtonCallback = std::function<void(const sf::Event::MouseButtonEvent &)>;
    using ScrollCallback = std::function<void(const sf::Event::MouseWheelScrollEvent &)>;
    using MoveCallback = std::function<void(const sf::Event::MouseMoveEvent &)>;
    using EnterLeaveCallback = std::function<void()>;

    enum class CallbackEvent
    {
        MouseWheelScrolled,
        MouseButtonPressed,
        MouseButtonReleased,
        MouseMoved,
        MouseEntered,
        MouseLeft
    };

public:
    // Set up call-back function in eventMgr
    Mouse() noexcept;
    ~Mouse() noexcept;
    Mouse(const Mouse &) = delete;
    Mouse &operator=(const Mouse &) = delete;

    // Move buttonMap into prev-buttonMap
    static void Update() noexcept;

    template <Mouse::CallbackEvent EventType, typename T>
    static void AddCallback(T callback) noexcept
    {
        if constexpr (EventType == CallbackEvent::MouseWheelScrolled)
        {
            m_scrollCallbacks[EventType].push_back(callback);
        }
        else if constexpr (EventType == CallbackEvent::MouseButtonPressed || EventType == CallbackEvent::MouseButtonReleased)
        {
            m_buttonCallbacks[EventType].push_back(callback);
        }
        else if constexpr (EventType == CallbackEvent::MouseMoved)
        {
            m_moveCallbacks[EventType].push_back(callback);
        }
        else if constexpr (EventType == CallbackEvent::MouseEntered || EventType == CallbackEvent::MouseLeft)
        {
            m_enterLeaveCallbacks[EventType].push_back(callback);
        }
    }

    static bool IsDown(const sf::Mouse::Button &button) noexcept;
    static bool IsPressed(const sf::Mouse::Button &button) noexcept;
    static bool IsReleased(const sf::Mouse::Button &button) noexcept;
    static bool AnyButtonDown() noexcept;

    static sf::Vector2f GetPos() noexcept { return m_mousePos; }
    static float GetVerticalScroll() noexcept { return m_verticalScrollBuffer; }
    static float GetHorizontalScroll() noexcept { return m_horizontalScrollBuffer; }

private:
    virtual void OnEvent(const sf::Event &event) noexcept override;

    static void OnPress(const sf::Event::MouseButtonEvent &event) noexcept;
    static void OnRelease(const sf::Event::MouseButtonEvent &event) noexcept;
    static void OnMove(const sf::Event::MouseMoveEvent &event) noexcept;
    static void OnEnter() noexcept;
    static void OnLeave() noexcept;
    static void OnScroll(const sf::Event::MouseWheelScrollEvent &event) noexcept;

private:
    static std::map<sf::Mouse::Button, bool> m_buttonmap;
    static std::map<sf::Mouse::Button, bool> m_prevButtonmap;
    static std::map<CallbackEvent, std::vector<ButtonCallback>> m_buttonCallbacks;
    static std::map<CallbackEvent, std::vector<ScrollCallback>> m_scrollCallbacks;
    static std::map<CallbackEvent, std::vector<MoveCallback>> m_moveCallbacks;
    static std::map<CallbackEvent, std::vector<EnterLeaveCallback>> m_enterLeaveCallbacks;

    static sf::Vector2f m_mousePos;
    static bool m_inWindow;

    static float m_verticalScrollBuffer;
    static float m_horizontalScrollBuffer;
};