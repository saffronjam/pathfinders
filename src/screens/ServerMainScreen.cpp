#include "ServerMainScreen.h"
#include "AppServer.h"

ServerMainScreen::ServerMainScreen(AppServer &parent)
    : m_parent(parent)
{
}

ServerMainScreen::~ServerMainScreen()
{
}

void ServerMainScreen::Build()
{
}

void ServerMainScreen::Destroy()
{
}

void ServerMainScreen::OnEntry()
{
    m_parent.GetServer().SetPort(1332);
    m_parent.GetServer().Open();
}

void ServerMainScreen::OnExit()
{
}

void ServerMainScreen::Update()
{
    if (Keyboard::IsPressed(sf::Keyboard::T))
    {
        log_info("Asking clients if they are alive... with TCP!");
        m_parent.GetServer().BroadcastEmpty<TCP>(AreYouAlive);
    }
    if (Keyboard::IsPressed(sf::Keyboard::U))
    {
        log_info("Asking clients if they are alive... with UDP!");
        m_parent.GetServer().BroadcastEmpty<UDP>(AreYouAlive);
    }

    if (Keyboard::IsPressed(sf::Keyboard::C))
    {
        log_info("Closing server!");
        m_parent.GetServer().Close();
    }
    if (Keyboard::IsReleased(sf::Keyboard::C))
    {
        log_info("Opening server!");
        m_parent.GetServer().Open();
    }
}

void ServerMainScreen::Draw()
{
}

int ServerMainScreen::GetNextScreenIndex() const
{
    return SCREENINDEX_NO_SCREEN;
}

int ServerMainScreen::GetPreviousScreenIndex() const
{
    return SCREENINDEX_NO_SCREEN;
}