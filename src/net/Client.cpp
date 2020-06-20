#include "Client.h"

Client::Client(sf::IpAddress address, sf::Uint16 port)
    : m_cachedAddress(address),
      m_cachedPort(port),
      m_connState(ConnState::Disconnected),
      m_autoReconnect(false)
{
    AddNetModule(std::make_unique<PingModule>(this));
    AddNetModule(std::make_unique<SetupModule>(this));
    StartListening();
}

Client::~Client()
{
    StopListening();
    if (m_connState != ConnState::Disconnected)
        Disconnect();
}

void Client::Connect()
{
    if (m_connState == ConnState::Connected)
    {
        THROW(Exception, "Tried to connect an already connected client: %s", "Try to disconnect before connecting");
    }
    if (m_cachedAddress == sf::IpAddress::None || m_cachedPort == 0)
    {
        THROW(Exception, "Tried to connect a non-configured client: %s", "IP-address and port must be configured with SetNet()");
    }

    CollectConnectorThread();
    ClearConnections();
    m_connState = ConnState::TryingToConnect;
    m_tryConnectDelay = sf::seconds(2.0f);
    m_connector = std::thread(&Client::ConnectThreadFn, this);
}

void Client::Disconnect()
{
    if (m_connState == ConnState::Disconnected)
    {
        THROW(Exception, "Tried to disconnect an already disconnected client: %s", "Try to connect before disconnecting");
    }
    CollectConnectorThread();
    ClearConnections();
    ClearInBuffer();
    m_connState = ConnState::Disconnected;
}

void Client::SetNet(sf::IpAddress address, sf::Uint16 port) noexcept
{
    m_cachedAddress = address;
    m_cachedPort = port;
}

void Client::ConnectThreadFn()
{
    if (!m_connectMutex.try_lock())
        return;
    ServerInfo info;
    Connection conn;
    for (; m_connState == ConnState::TryingToConnect; sf::sleep(m_tryConnectDelay))
    {
        try
        {
            if (conn.GetTcpSocket().connect(m_cachedAddress, m_cachedPort) != sf::Socket::Status::Done)
            {
                THROW(Exception, "Could not connect TCP-socket to remote host: %s:%u", m_cachedAddress.toString().c_str(), m_cachedPort);
            }
            if (conn.GetUdpSocket().bind(sf::Socket::AnyPort) != sf::Socket::Status::Done)
            {
                THROW(Exception, "Could not bind UDP-socket. Port: %s", "Any available port");
            }
            conn.SetUdpRemoteAddress(m_cachedAddress);
            conn.SetUdpRemotePort(m_cachedPort);

            AddConnection(conn, &info);
            m_connState = ConnState::Connected;
            m_connectMutex.unlock();
            return;
        }
        LogOnly;
        conn.GetTcpSocket().disconnect();
        conn.GetUdpSocket().unbind();
    };
    conn.Destroy();
    m_connectMutex.unlock();
}

void Client::CollectConnectorThread()
{
    m_connState = ConnState::Disconnected;
    m_tryConnectDelay = sf::seconds(0.0f);
    if (m_connector.joinable())
        m_connector.join();
}

void Client::HandleClosedConnection(NetUID uid)
{
    if (m_removeConnMutex.try_lock())
    {
        if (m_connState == ConnState::Connected)
        {
            Disconnect();
            if (m_autoReconnect)
                Connect();
        }
        m_removeConnMutex.unlock();
    }
}