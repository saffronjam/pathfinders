#pragma once

#include <mutex>
#include <thread>

#include <SFML/System/Sleep.hpp>

#include "IConnHandler.h"
#include "ClientInfo.h"
#include "GenericThrowMacros.h"
#include "Log.h"
#include "PingModule.h"

class Server : public IConnHandler
{
public:
    Server(sf::Uint16 port = 0);
    ~Server();
    Server(const Server &) = delete;
    const Server &operator()(const Server &) = delete;

    void Open();
    void Close();

    void SetPort(sf::Uint16 port) noexcept { m_port = port; }

    template <Protocol P, typename T>
    void Broadcast(PacketType type, const T &data);
    template <Protocol P>
    void BroadcastEmpty(PacketType type);
    template <Protocol P, typename T>
    void BroadcastArray(PacketType type, const T *data, int nElements);
    template <Protocol P>
    void BroadcastRaw(PacketType type, const sf::Uint8 *data, size_t size);

private:
    void OpenThreadFn();
    void CollectOpenerThread();

    NetUID GenerateUID() noexcept override;
    void ResetUIDGenerator() noexcept { m_nextUID = 1; }; // Start on UID = 1, since 0 is invalid

private:
    sf::Uint16 m_port;

    std::thread m_opener;
    std::mutex m_openMutex;
    sf::Time m_tryOpenDelay;
    enum class ConnState
    {
        Opened,
        Closed,
        TryingToOpen
    } m_connState;

    NetUID m_nextUID;
};

template <Protocol P, typename T>
void Server::Broadcast(PacketType type, const T &data)
{
    for (auto &[uid, pair] : GetConnections())
        INetMgr::Send<P>(type, data, &pair.first);
}
template <Protocol P>
void Server::BroadcastEmpty(PacketType type)
{
    for (auto &[uid, pair] : GetConnections())
        INetMgr::SendEmpty<P>(type, &pair.first);
}
template <Protocol P, typename T>
void Server::BroadcastArray(PacketType type, const T *data, int nElements)
{
    for (auto &[uid, pair] : GetConnections())
        INetMgr::SendArray<P>(type, data, nElements, &pair.first);
}
template <Protocol P>
void Server::BroadcastRaw(PacketType type, const sf::Uint8 *data, size_t size)
{
    for (auto &[uid, pair] : GetConnections())
        INetMgr::SendRaw<P>(type, data, size, &pair.first);
}