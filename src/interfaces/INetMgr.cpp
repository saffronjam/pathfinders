#include "INetMgr.h"

INetMgr::INetMgr()
    : m_uid(0),
      m_listening(false)
{
    PacketMgr::AddPacketBuffer(&m_inBuffer, &m_inBufferLock);
}

INetMgr::~INetMgr()
{
    StopListening();
    m_inBuffer.clear();
}

void INetMgr::StartListening()
{
    m_listening = true;
    m_listenerThread = std::thread(&INetMgr::ListenerThreadFn, this);
}

void INetMgr::StopListening()
{
    m_listening = false;
    if (m_listenerThread.joinable())
        m_listenerThread.join();
}

void INetMgr::ReceiveTCP(const Connection *conn)
{
    try
    {
        sf::Packet incoming;
        std::optional<IConnInfo *> connInfoOpt = std::nullopt;
        std::optional<sf::Uint64> uidOpt = std::nullopt;
        auto &socket = conn->GetTcpSocket();

        // Make sure connInfo is available from start
        if (!(connInfoOpt = GetConnInfoByConnection(conn)).has_value())
            THROW(Exception, "No ConnInfo was found for connection behind TCP-packet: %s:%u", socket.getRemoteAddress().toString().c_str(), socket.getRemotePort());

        sf::Socket::Status status;
        while ((status = socket.receive(incoming)) != sf::Socket::Done)
        {
            if (status == sf::Socket::Error)
            {
                THROW(Exception, "Failed to receive TCP-packet: %s:%u", socket.getRemoteAddress().toString().c_str(), socket.getRemotePort());
            }
            else if (status == sf::Socket::Disconnected)
            {
                HandleClosedConnection(connInfoOpt.value()->GetUID());
                return;
            }
        }
        if (incoming.getDataSize() < sizeof(PacketType))
            THROW(Exception, "Bad data size in TCP-packet: %s:%u", socket.getRemoteAddress().toString().c_str(), socket.getRemotePort());

        // Make sure an UID is present in the packet
        if (!(uidOpt = Packager::GetUIDFromRawPacket(incoming)).has_value())
            THROW(Exception, "UID was not found in TCP-packet: %s:%u", socket.getRemoteAddress().toString().c_str(), socket.getRemotePort());
        // Make sure the UID is valid
        if (uidOpt.value() == 0)
            THROW(Exception, "UID was 0 in TCP-packet: %s:%u", socket.getRemoteAddress().toString().c_str(), socket.getRemotePort());

        std::optional<ParsedPacket> parseAttempt;
        parseAttempt = Packager::Parse<TCP>(incoming, conn, connInfoOpt.value());
        if (parseAttempt.has_value())
        {
            m_inBufferLock.lock();
            m_inBuffer.push_back(parseAttempt.value());
            m_inBufferLock.unlock();
        }
    }
    LogOnly;
}

void INetMgr::ReceiveUDP(sf::UdpSocket *udpListener)
{
    try
    {
        sf::Packet incoming;
        std::optional<IConnInfo *> connInfoOpt = std::nullopt;
        std::optional<sf::Uint64> uidOpt = std::nullopt;

        sf::IpAddress address;
        sf::Uint16 port;
        sf::Socket::Status status;
        while ((status = udpListener->receive(incoming, address, port)) != sf::Socket::Done)
        {
            if (status == sf::Socket::Error)
                THROW(Exception, "Failed to receive UDP-packet: %s:%u", address.toString().c_str(), port);
        }

        if (incoming.getDataSize() < sizeof(PacketType))
            THROW(Exception, "Bad data size in UDP-packet: %s:%u", address.toString().c_str(), port);

        // Make sure an UID is present in the packet
        if (!(uidOpt = Packager::GetUIDFromRawPacket(incoming)).has_value())
            THROW(Exception, "UID was not found in UDP-packet: %s:%u", address.toString().c_str(), port);
        // Make sure the UID is valid
        if (uidOpt.value() == 0)
            THROW(Exception, "UID was 0 in UDP-packet: %s:%u", address.toString().c_str(), port);
        // Make sure connInfo is available
        if (!(connInfoOpt = GetConnInfoByUID(uidOpt.value())).has_value())
            THROW(Exception, "No ConnInfo was found for UDP-packet: %s:%u", address.toString().c_str(), port);

        NewUdpConnection(uidOpt.value(), address, port);

        // Find the corresponding connection with the ID from the the UDP-packet
        auto connOpt = GetConnectionByUID(uidOpt.value());
        if (!connOpt.has_value())
            THROW(Exception, "No Connection was found for UDP-packet: %s:%u", address.toString().c_str(), port);

        std::optional<ParsedPacket> parseAttempt;
        parseAttempt = Packager::Parse<UDP>(incoming, connOpt.value(), connInfoOpt.value());
        if (parseAttempt.has_value())
        {
            m_inBufferLock.lock();
            m_inBuffer.push_back(parseAttempt.value());
            m_inBufferLock.unlock();
        }
    }
    LogOnly;
}

void INetMgr::AddToSocketSelector(const Connection *conn)
{
    m_connectionRefs.emplace(conn);
    m_socketSelector.add(conn->GetTcpSocket());
    if (!conn->IsUdpParent())
        m_socketSelector.add(conn->GetUdpSocket());
}

void INetMgr::AddToSocketSelector(sf::TcpListener *listener)
{
    m_tcpListenerRefs.emplace(listener);
    m_socketSelector.add(*listener);
}

void INetMgr::AddToSocketSelector(sf::UdpSocket *listener)
{
    m_udpListenerRefs.emplace(listener);
    m_socketSelector.add(*listener);
}

void INetMgr::RemoveFromSocketSelector(const Connection *conn)
{
    m_connectionRefs.erase(conn);
    m_socketSelector.remove(conn->GetTcpSocket());
    if (!conn->IsUdpParent())
        m_socketSelector.remove(conn->GetUdpSocket());
}

void INetMgr::RemoveFromSocketSelector(sf::TcpListener *listener)
{
    m_tcpListenerRefs.erase(listener);
    m_socketSelector.remove(*listener);
}

void INetMgr::RemoveFromSocketSelector(sf::UdpSocket *listener)
{
    m_udpListenerRefs.erase(listener);
    m_socketSelector.remove(*listener);
}

void INetMgr::ClearSocketSelector()
{
    m_connectionRefs.clear();
    m_tcpListenerRefs.clear();
    m_udpListenerRefs.clear();
    m_socketSelector.clear();
}

void INetMgr::AddNetModule(std::unique_ptr<INetModule> netModule)
{
    auto res = m_netModules.emplace(std::move(netModule));
    PacketMgr::AddHandler(res.first->get());
}

void INetMgr::ListenerThreadFn()
{
    while (m_listening)
    {
    START:
        bool anySocketReady = m_socketSelector.wait(sf::milliseconds(100));
        if (anySocketReady)
        {
            for (auto &listener : m_tcpListenerRefs)
            {
                if (m_socketSelector.isReady(*listener))
                {
                    NewTcpConnection(listener);
                    goto START;
                }
            }
            for (auto &listener : m_udpListenerRefs)
            {
                if (m_socketSelector.isReady(*listener))
                {
                    ReceiveUDP(listener);
                    goto START;
                }
            }
            for (auto &conn : m_connectionRefs)
            {
                if (m_socketSelector.isReady(conn->GetTcpSocket()))
                {
                    ReceiveTCP(conn);
                    goto START;
                }
                else if (!conn->IsUdpParent() && m_socketSelector.isReady(conn->GetUdpSocket()))
                {
                    ReceiveUDP(&conn->GetUdpSocket());
                    break;
                }
            }
        }
    }
}

INetMgr::Exception::Exception(int line, const char *file, const char *errorString) noexcept
    : IException(line, file),
      errorString(errorString)
{
}

const char *INetMgr::Exception::what() const noexcept
{
    std::ostringstream oss;
    oss << "[Type] " << GetType() << std::endl
        << "[Description] " << GetErrorString() << std::endl
        << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}
const char *INetMgr::Exception::GetType() const noexcept
{
    return "V-2DFramework NetMgr Exception";
}
const char *INetMgr::Exception::GetErrorString() const noexcept
{
    return errorString.c_str();
}