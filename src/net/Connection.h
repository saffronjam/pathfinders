#pragma once

#include <optional>

#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/UdpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>

#include "Log.h"

// A wrapper around TCP- and UDP-socket
class Connection
{
public:
    Connection(const sf::IpAddress &udpRemoteAddress = sf::IpAddress::None, const sf::Uint16 &udpRemotePort = 0);
    Connection(sf::UdpSocket *udpSocket, const sf::IpAddress &udpRemoteAddress = sf::IpAddress::None, const sf::Uint16 &udpRemotePort = 0);
    ~Connection();

    void Destroy();

    sf::TcpSocket &GetTcpSocket() const { return *m_tcpSocket; }
    sf::UdpSocket &GetUdpSocket() const { return m_udpSocket.has_value() ? *m_udpSocket.value() : *m_udpSocketChild.value(); }
    sf::IpAddress GetUdpRemoteAddress() const noexcept { return m_udpRemoteAddress; }
    sf::Uint16 GetUdpRemotePort() const noexcept { return m_udpRemotePort; }

    void SetUdpRemoteAddress(const sf::IpAddress &address) noexcept { m_udpRemoteAddress = address; }
    void SetUdpRemotePort(const sf::Uint16 &port) noexcept { m_udpRemotePort = port; }

    bool IsUdpParent() const noexcept { return m_udpSocketChild.has_value(); }

private:
    sf::TcpSocket *m_tcpSocket;
    std::optional<sf::UdpSocket *> m_udpSocket;
    std::optional<sf::UdpSocket *> m_udpSocketChild;

    sf::IpAddress m_udpRemoteAddress;
    sf::Uint16 m_udpRemotePort;
};