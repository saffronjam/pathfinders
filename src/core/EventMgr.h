#pragma once

#include <functional>
#include <vector>

#include <SFML/Window/Event.hpp>

#include "Window.h"

struct EventHandler
{
    virtual void OnEvent(const sf::Event &) = 0;
};

class EventMgr
{
public:
    static void PollAll();
    static void AddHandler(EventHandler *handler);

private:
    static std::vector<EventHandler *> m_callbacks;
};