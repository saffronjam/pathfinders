#include "Server.h"

Server::Server(sf::Uint16 port)
    : m_port(port),
      m_connState(ConnState::Closed)
{
    ResetUIDGenerator();
    SetUID(1); // Server has a static UID of 1
    AddNetModule(std::unique_ptr<PingModule>(new PingModule(this)));
    StartListening();
}

Server::~Server()
{
    StopListening();
    if (m_connState != ConnState::Closed)
        Close();
}

void Server::Open()
{
    if (m_connState == ConnState::Opened)
    {
        THROW(Exception, "Tried to open an already opened server: %s", "Try to close before opening the server");
    }
    if (m_port == 0)
    {
        THROW(Exception, "Tried to open a non-configured server. Port: %u", m_port);
    }

    CollectOpenerThread();
    m_connState = ConnState::TryingToOpen;
    m_tryOpenDelay = sf::seconds(2.0f);
    m_opener = std::thread(&Server::OpenThreadFn, this);
}

void Server::Close()
{
    if (m_connState == ConnState::Closed)
    {
        THROW(Exception, "Tried to close an already closed server: %s", "Try to open before closing the server");
    }
    CollectOpenerThread();
    CloseTcpListener();
    CloseUdpListener();
    ClearConnections();
    ResetUIDGenerator();
    ClearInBuffer();
    m_connState = ConnState::Closed;
}

void Server::OpenThreadFn()
{
    if (!m_openMutex.try_lock())
        return;
    for (; m_connState == ConnState::TryingToOpen; sf::sleep(m_tryOpenDelay))
    {
        try
        {
            OpenTcpListener(m_port);
            OpenUdpListener(m_port);
            m_connState = ConnState::Opened;
            m_openMutex.unlock();
            return;
        }
        LogOnly;
        CloseTcpListener();
        CloseTcpListener();
        m_openMutex.unlock();
    };
}

void Server::CollectOpenerThread()
{
    m_connState == ConnState::Closed;
    m_tryOpenDelay = sf::seconds(0.0f);
    if (m_opener.joinable())
        m_opener.join();
}

NetUID Server::GenerateUID() noexcept
{
    return m_nextUID++;
}
