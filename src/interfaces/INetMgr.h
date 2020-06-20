#pragma once

#include <vector>
#include <map>
#include <set>
#include <mutex>
#include <thread>
#include <memory>

#include <SFML/Network/SocketSelector.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/Network/TcpListener.hpp>

#include "Protocol.h"
#include "IException.h"
#include "INetModule.h"
#include "IConnInfo.h"
#include "Packager.h"
#include "PacketMgr.h"
#include "NetUID.h"
#include "GenericThrowMacros.h"
#include "Log.h"

class INetMgr
{
public:
    INetMgr();
    ~INetMgr();

    void StartListening();
    void StopListening();

    template <Protocol P, typename T>
    void Send(PacketType type, const T &data, const Connection *conn);
    template <Protocol P>
    void SendEmpty(PacketType type, const Connection *conn);
    template <Protocol P, typename T>
    void SendArray(PacketType type, const T *data, int nElements, const Connection *conn);
    template <Protocol P>
    void SendRaw(PacketType type, const sf::Uint8 *data, size_t size, const Connection *conn);

    void SetUID(sf::Uint64 uid) noexcept { m_uid = uid; }
    sf::Uint64 GetUID() const noexcept { return m_uid; }

protected:
    // Receives incoming TCP packet from given Connection
    // This function is only called from the socket selector thread
    void ReceiveTCP(const Connection *conn);
    // Receives incoming UDP packet from given UDP-socket
    // This function is only called from the socket selector thread
    void ReceiveUDP(sf::UdpSocket *udpListener);
    // Adds socket to socket selector depending on
    void AddToSocketSelector(const Connection *conn);
    // Adds listener to socket selector
    void AddToSocketSelector(sf::TcpListener *listener);
    // Adds listener to socket selector
    void AddToSocketSelector(sf::UdpSocket *listener);
    // Removes both TCP- and UDP-socket from socket selector
    // UDP-socket is only remove if the connection is not a UDP-parent
    void RemoveFromSocketSelector(const Connection *conn);
    // Removes listener from socket selector
    void RemoveFromSocketSelector(sf::TcpListener *listener);
    // Removes listener from socket selector
    void RemoveFromSocketSelector(sf::UdpSocket *listener);
    // Clears all entries in socket selector
    void ClearSocketSelector();
    // Adds NetModule by taking ownership of the unique_ptr with std::move (Parameter will be nullptr after call)
    void AddNetModule(std::unique_ptr<INetModule> netModule);
    // Clears every buffered packet (independant of protocol)
    void ClearInBuffer() { m_inBuffer.clear(); }

    virtual void NewTcpConnection(sf::TcpListener *listener){};
    virtual void NewUdpConnection(NetUID uid, const sf::IpAddress address, const sf::Uint16 &port){};
    virtual void HandleClosedConnection(NetUID uid) = 0;
    virtual std::optional<Connection *> GetConnectionByUID(NetUID uid) = 0;
    virtual std::optional<IConnInfo *> GetConnInfoByUID(NetUID uid) = 0;
    virtual std::optional<IConnInfo *> GetConnInfoByConnection(const Connection *conn) = 0;

private:
    template <Protocol P>
    void SendOut(sf::Packet &packet, const Connection *conn);
    void ListenerThreadFn();

private:
    sf::Uint64 m_uid;

    bool m_listening;
    std::thread m_listenerThread;
    sf::SocketSelector m_socketSelector;

    PacketBuffer m_inBuffer;
    std::mutex m_inBufferLock;

    std::set<std::unique_ptr<INetModule>> m_netModules;
    std::set<sf::TcpListener *> m_tcpListenerRefs;
    std::set<sf::UdpSocket *> m_udpListenerRefs;
    std::set<const Connection *> m_connectionRefs;

public:
    class Exception : public IException
    {
    public:
        Exception(int line, const char *file, const char *errorString) noexcept;
        const char *what() const noexcept override;
        virtual const char *GetType() const noexcept override;
        const char *GetErrorString() const noexcept;

    private:
        std::string errorString;
    };
};

template <Protocol P, typename T>
void INetMgr::Send(PacketType type, const T &data, const Connection *conn)
{
    if (m_uid != 0)
    {
        sf::Packet outgoing = Packager::Pack(type, m_uid, reinterpret_cast<const sf::Uint8 *>(&data), sizeof(T));
        SendOut<P>(outgoing, conn);
    }
}
template <Protocol P>
void INetMgr::SendEmpty(PacketType type, const Connection *conn)
{
    if (m_uid != 0)
    {
        sf::Packet outgoing = Packager::Pack(type, m_uid, nullptr, 0);
        SendOut<P>(outgoing, conn);
    }
}
template <Protocol P, typename T>
void INetMgr::SendArray(PacketType type, const T *data, int nElements, const Connection *conn)
{
    if (m_uid != 0)
    {
        sf::Packet outgoing = Packager::Pack(type, m_uid, reinterpret_cast<const sf::Uint8 *>(data), sizeof(T) * nElements);
        SendOut<P>(outgoing, conn);
    }
}
template <Protocol P>
void INetMgr::SendRaw(PacketType type, const sf::Uint8 *data, size_t size, const Connection *conn)
{
    if (m_uid != 0)
    {
        sf::Packet outgoing = Packager::Pack(type, m_uid, data, size);
        SendOut<P>(outgoing, conn);
    }
}

template <Protocol P>
void INetMgr::SendOut(sf::Packet &packet, const Connection *conn)
{
    try
    {
        if constexpr (P == Protocol::TCP)
        {
            sf::Socket::Status status;
            while ((status = conn->GetTcpSocket().send(packet)) != sf::Socket::Done)
            {
                if (status == sf::Socket::Status::Error)
                {
                    auto &socket = conn->GetTcpSocket();
                    THROW(Exception, "Failed to send TCP-packet: %s:%u", socket.getRemoteAddress().toString().c_str(), socket.getRemotePort());
                }
                else if (status == sf::Socket::Status::Disconnected)
                {
                    auto &socket = conn->GetTcpSocket();
                    THROW(Exception, "Failed to send TCP-packet because recipient disconnected: %s:%u", socket.getRemoteAddress().toString().c_str(), socket.getRemotePort());
                }
            }
        }
        else if constexpr (P == Protocol::UDP)
        {
            sf::IpAddress address = conn->GetUdpRemoteAddress();
            sf::Uint16 port = conn->GetUdpRemotePort();
            sf::Socket::Status status;
            while ((status = conn->GetUdpSocket().send(packet, address, port)) != sf::Socket::Done)
            {
                if (status == sf::Socket::Status::Error)
                {
                    THROW(Exception, "Failed to send UDP-packet: %s:%u", address.toString().c_str(), port);
                }
                else if (status == sf::Socket::Status::Disconnected)
                {
                    THROW(Exception, "Failed to send UDP-packet because recipient disconnected: %s:%u", address.toString().c_str(), port);
                }
            }
        }
        else
        {
            THROW(Exception, "Tried to send a packet with invalid protocol: %u", P);
        }
    }
    LogOnly;
}
