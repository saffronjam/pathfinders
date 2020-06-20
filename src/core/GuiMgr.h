#pragma once

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Desktop.hpp>
#include <SFGUI/Widget.hpp>

#include "Clock.h"
#include "EventMgr.h"

class GuiMgr : public EventHandler
{
public:
    GuiMgr();

    static void Update() noexcept { m_desktop.Update(Clock::Delta().asSeconds()); };
    static void Draw() noexcept { m_sfgui.Display(*Window::GetSFWindow()); }

    static void Add(const sfg::Widget::Ptr &widget) noexcept { m_desktop.Add(widget); };
    static void Remove(const sfg::Widget::Ptr &widget) noexcept { m_desktop.Remove(widget); }
    static void ClearWidgets() noexcept
    {
        m_desktop.RemoveAll();
        m_desktop.Refresh();
    };

private:
    virtual void OnEvent(const sf::Event &event) noexcept override;

private:
    static sfg::SFGUI m_sfgui;

    static sfg::Desktop m_desktop;
};