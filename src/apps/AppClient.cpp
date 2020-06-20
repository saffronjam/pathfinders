#include "AppClient.h"

void AppClient::OnInit()
{
    Window::SetTitle("Client");
}

void AppClient::OnExit()
{
}

void AppClient::OnPreUpdate()
{
}

void AppClient::OnPostUpdate()
{
}

void AppClient::AddScreens()
{
    m_mainScreen = std::make_unique<ClientMainScreen>(*this);
    m_screenList->AddScreen(m_mainScreen.get());
    m_screenList->SetCurrentScreen(m_mainScreen->GetScreenIndex());
}