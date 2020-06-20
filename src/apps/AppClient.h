#pragma once

#include "memory"

#include "IApp.h"
#include "ClientMainScreen.h"
#include "Client.h"

class AppClient : public IApp
{
public:
    virtual void OnInit() override;
    virtual void OnExit() override;
    virtual void OnPreUpdate() override;
    virtual void OnPostUpdate() override;
    virtual void AddScreens() override;

    Client &GetClient() noexcept { return m_client; }

private:
    std::unique_ptr<ClientMainScreen> m_mainScreen;
    Client m_client;
};