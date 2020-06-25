#include "IConnHandler.h"

#include "ClientInfo.h"
#include "ServerInfo.h"

void IConnHandler::AddConnection(const Connection &connection, IConnInfo *connInfo)
{
    if (connInfo->GetUID() == 0)
        return;

    ServerInfo *serverInfo = nullptr;
    ClientInfo *clientInfo = nullptr;
    if ((serverInfo = dynamic_cast<ServerInfo *>(connInfo)))
    {
        serverInfo = new ServerInfo(*serverInfo);
        auto impRes = m_connections.emplace(std::make_pair(connInfo->GetUID(), std::make_pair(connection, serverInfo)));
        if (impRes.second)
            AddToSocketSelector(&impRes.first->second.first);
    }
    else if ((clientInfo = dynamic_cast<ClientInfo *>(connInfo)))
    {
        clientInfo = new ClientInfo(*clientInfo);
        auto impRes = m_connections.emplace(std::make_pair(connInfo->GetUID(), std::make_pair(connection, clientInfo)));
        if (impRes.second)
            AddToSocketSelector(&impRes.first->second.first);
    }
}

void IConnHandler::RemoveConnection(NetUID uid)
{
    auto findRes = m_connections.find(uid);
    if (findRes != m_connections.end())
    {
        RemoveFromSocketSelector(&findRes->second.first);
        delete m_connections[uid].second;
        findRes->second.first.GetTcpSocket().disconnect();
        if (!findRes->second.first.IsUdpParent())
            findRes->second.first.GetUdpSocket().unbind();
        m_connections.erase(uid);
    }
}

void IConnHandler::ClearConnections()
{
    ClearSocketSelector();
    for (auto &[key, pair] : m_connections)
    {
        delete pair.second;
        pair.first.GetTcpSocket().disconnect();
        if (!pair.first.IsUdpParent())
            pair.first.GetUdpSocket().unbind();
        pair.first.Destroy();
    }
    m_connections.clear();
}

void IConnHandler::OpenTcpListener(sf::Uint16 port)
{
    if (m_tcpListener.has_value())
    {
        THROW(Exception, "Tried to start an already started TCP listener. Port: %u", port);
    }
    m_tcpListener = new sf::TcpListener();

    if (m_tcpListener.value()->listen(port) != sf::Socket::Status::Done)
    {
        THROW(Exception, "Failed to start listener on server. Port: %u", port);
    }
    AddToSocketSelector(m_tcpListener.value());
}

void IConnHandler::OpenUdpListener(sf::Uint16 port)
{
    if (m_udpListener.has_value())
    {
        THROW(Exception, "Tried to start an already started TCP listener. Port: %u", port);
    }
    m_udpListener = new sf::UdpSocket();

    if (m_udpListener.value()->bind(port) != sf::Socket::Status::Done)
    {
        THROW(Exception, "Could not bind UDP-socket. Port: %u", port);
    }
    AddToSocketSelector(m_udpListener.value());
}

void IConnHandler::CloseTcpListener() noexcept
{
    RemoveFromSocketSelector(m_tcpListener.value());
    m_tcpListener.value()->close();
    delete m_tcpListener.value();
    m_tcpListener.reset();
}

void IConnHandler::CloseUdpListener() noexcept
{
    RemoveFromSocketSelector(m_udpListener.value());
    m_udpListener.value()->unbind();
    delete m_udpListener.value();
    m_udpListener.reset();
}

void IConnHandler::NewTcpConnection(sf::TcpListener *listener)
{
    if (!m_tcpListener.has_value() || !m_udpListener.has_value())
    {
        THROW(Exception, "Could not accept new TCP connection because one the listeners was not opened: TCP-status: %s, UDP-status: %s",
              m_tcpListener.has_value() ? "Open" : "Closed", m_udpListener.has_value() ? "Open" : "Closed");
    }

    // Every received tcp-connection is parent over server udp-socket
    Connection newConn(m_udpListener.value());
    if (listener->accept(newConn.GetTcpSocket()) != sf::Socket::Status::Done)
        return;

    sf::Uint64 uid = GenerateUID();

    // ClientInfo is always used if the connection handles a tcp-listener
    ClientInfo info(uid);
    AddConnection(newConn, &info);

    // Giving out UID to new connnection
    Send<TCP>(UID, uid, &newConn);
}

void IConnHandler::NewUdpConnection(sf::Uint64 uid, const sf::IpAddress address, const sf::Uint16 &port)
{
    try
    {
        auto connOpt = GetConnectionByUID(uid);
        if (!connOpt.has_value())
            THROW(Exception, "UDP address update contained UID that did not correspond to any connection. UID: %llu", uid);

        // Update the remote address
        Connection *conn = const_cast<Connection *>(connOpt.value());
        conn->SetUdpRemoteAddress(address);
        conn->SetUdpRemotePort(port);
    }
    LogOnly;
}

void IConnHandler::HandleClosedConnection(NetUID uid)
{
    RemoveConnection(uid);
}

std::optional<Connection *> IConnHandler::GetConnectionByIndex(size_t index)
{
    if (index < m_connections.size())
    {
        auto resultIter = m_connections.begin();
        std::advance(resultIter, index);
        return &resultIter->second.first;
    }
    return std::nullopt;
}

std::optional<Connection *> IConnHandler::GetConnectionByUID(NetUID uid)
{
    auto findRes = m_connections.find(uid);
    return findRes == m_connections.end() ? std::nullopt : std::optional<Connection *>(&findRes->second.first);
}

std::optional<IConnInfo *> IConnHandler::GetConnInfoByUID(NetUID uid)
{
    auto findRes = m_connections.find(uid);
    return findRes == m_connections.end() ? std::nullopt : std::optional<IConnInfo *>(findRes->second.second);
}

std::optional<IConnInfo *> IConnHandler::GetConnInfoByConnection(const Connection *conn)
{
    auto findRes = std::find_if(m_connections.begin(), m_connections.end(), [this, &conn](const auto &pair) {
        return &pair.second.first == conn;
    });

    return findRes == m_connections.end() ? std::nullopt : std::optional<IConnInfo *>(findRes->second.second);
}