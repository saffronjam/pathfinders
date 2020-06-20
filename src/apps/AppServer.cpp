#include "AppServer.h"

void AppServer::OnInit()
{
    Window::SetTitle("Server");
}

void AppServer::OnExit()
{
}

void AppServer::OnPreUpdate()
{
}

void AppServer::OnPostUpdate()
{
}

void AppServer::AddScreens()
{
    m_mainScreen = std::make_unique<ServerMainScreen>(*this);
    m_screenList->AddScreen(m_mainScreen.get());
    m_screenList->SetCurrentScreen(m_mainScreen->GetScreenIndex());
}