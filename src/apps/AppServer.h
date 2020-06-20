#pragma once

#include "memory"

#include "IApp.h"
#include "ServerMainScreen.h"
#include "Server.h"

class AppServer : public IApp
{
public:
    virtual void OnInit() override;
    virtual void OnExit() override;
    virtual void OnPreUpdate() override;
    virtual void OnPostUpdate() override;
    virtual void AddScreens() override;

    Server &GetServer() noexcept { return m_server; }

private:
    std::unique_ptr<ServerMainScreen> m_mainScreen;
    Server m_server;
};