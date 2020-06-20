#pragma once

#include "INetMgr.h"
#include "IConnInfo.h"
#include "Connection.h"
#include "NetUID.h"

#define SERVER_UID 1

class IConnHandler : public INetMgr
{
protected:
    void AddConnection(const Connection &connection, IConnInfo *connInfo);
    void RemoveConnection(NetUID uid);
    void ClearConnections();

    void OpenTcpListener(sf::Uint16 port);
    void OpenUdpListener(sf::Uint16 port);
    void CloseTcpListener() noexcept;
    void CloseUdpListener() noexcept;

    virtual void NewTcpConnection(sf::TcpListener *listener) override;
    virtual void NewUdpConnection(NetUID uid, const sf::IpAddress address, const sf::Uint16 &port) override;
    virtual void HandleClosedConnection(NetUID uid) override;

    virtual NetUID GenerateUID() noexcept { return 1; }

    std::optional<Connection *> GetConnectionByIndex(size_t index);
    std::optional<Connection *> GetConnectionByUID(NetUID uid) override;
    std::optional<IConnInfo *> GetConnInfoByUID(NetUID uid) override;
    std::optional<IConnInfo *> GetConnInfoByConnection(const Connection *conn) override;
    auto &GetConnections() const noexcept { return m_connections; }

private:
    std::map<NetUID, std::pair<Connection, IConnInfo *>> m_connections;
    std::optional<sf::TcpListener *> m_tcpListener;
    std::optional<sf::UdpSocket *> m_udpListener;
};