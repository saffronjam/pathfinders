#include "GuiMgr.h"

sfg::SFGUI GuiMgr::m_sfgui;
sfg::Desktop GuiMgr::m_desktop;

GuiMgr::GuiMgr()
{
    EventMgr::AddHandler(this);
}

void GuiMgr::OnEvent(const sf::Event &event) noexcept
{
    m_desktop.HandleEvent(event);
}