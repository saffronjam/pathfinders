#pragma once

#include <mutex>
#include <thread>

#include <SFML/System/Sleep.hpp>

#include "Log.h"
#include "IConnHandler.h"
#include "ServerInfo.h"
#include "PingModule.h"
#include "SetupModule.h"
#include "GenericThrowMacros.h"

class Client : public IConnHandler
{
public:
    enum class ConnState
    {
        Disconnected,
        Connected,
        TryingToConnect
    };

public:
    Client(sf::IpAddress address = sf::IpAddress::None, sf::Uint16 port = 0);
    ~Client();
    Client(const Client &) = delete;
    const Client &operator()(const Client &) = delete;

    void Connect();
    void Disconnect();

    void EnableAutoReconnect() noexcept { m_autoReconnect = true; }
    void DisableAutoReconnect() noexcept { m_autoReconnect = false; }

    void SetNet(sf::IpAddress address, sf::Uint16 port) noexcept;

    template <Protocol P, typename T>
    void Send(PacketType type, const T &data);
    template <Protocol P>
    void SendEmpty(PacketType type);
    template <Protocol P, typename T>
    void SendArray(PacketType type, const T *data, int nElements);
    template <Protocol P>
    void SendRaw(PacketType type, const sf::Uint8 *data, size_t size);

private:
    void ConnectThreadFn();
    void CollectConnectorThread();

    void HandleClosedConnection(NetUID uid) override;

private:
    sf::IpAddress m_cachedAddress;
    sf::Uint16 m_cachedPort;

    std::thread m_connector;
    std::mutex m_connectMutex;
    sf::Time m_tryConnectDelay;
    ConnState m_connState;

    std::mutex m_removeConnMutex;

    bool m_autoReconnect;
};

template <Protocol P, typename T>
void Client::Send(PacketType type, const T &data)
{
    if (m_connState == ConnState::Connected)
    {
        auto conn = GetConnectionByUID(SERVER_UID);
        if (conn.has_value())
            INetMgr::Send<P>(type, data, conn.value());
    }
}
template <Protocol P>
void Client::SendEmpty(PacketType type)
{
    if (m_connState == ConnState::Connected)
    {
        auto conn = GetConnectionByUID(SERVER_UID);
        if (conn.has_value())
            INetMgr::SendEmpty<P>(type, conn.value());
    }
}
template <Protocol P, typename T>
void Client::SendArray(PacketType type, const T *data, int nElements)
{
    if (m_connState == ConnState::Connected)
    {
        auto conn = GetConnectionByUID(SERVER_UID);
        if (conn.has_value())
            INetMgr::SendArray<P>(type, data, nElements, conn.value());
    }
}
template <Protocol P>
void Client::SendRaw(PacketType type, const sf::Uint8 *data, size_t size)
{
    if (m_connState == ConnState::Connected)
    {
        auto conn = GetConnectionByUID(SERVER_UID);
        if (conn.has_value())
            INetMgr::SendRaw<P>(type, data, size, conn.value());
    }
}