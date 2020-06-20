#include "Connection.h"

Connection::Connection(const sf::IpAddress &udpRemoteAddress, const sf::Uint16 &udpRemotePort)
    : m_tcpSocket(new sf::TcpSocket()),
      m_udpSocket(new sf::UdpSocket()),
      m_udpSocketChild(std::nullopt),
      m_udpRemoteAddress(udpRemoteAddress),
      m_udpRemotePort(udpRemotePort)
{
}

Connection::Connection(sf::UdpSocket *udpSocket, const sf::IpAddress &udpRemoteAddress, const sf::Uint16 &udpRemotePort)
    : m_tcpSocket(new sf::TcpSocket()),
      m_udpSocketChild(udpSocket),
      m_udpRemoteAddress(udpRemoteAddress),
      m_udpRemotePort(udpRemotePort)
{
}

Connection::~Connection()
{
  m_tcpSocket = nullptr;
  m_udpSocket = std::nullopt;
  m_udpSocketChild = std::nullopt;
}

void Connection::Destroy()
{
  if (m_tcpSocket)
    delete m_tcpSocket;
  if (!IsUdpParent() && m_udpSocket.value())
    delete m_udpSocket.value();

  m_tcpSocket = nullptr;
  m_udpSocket.reset();
  m_udpSocketChild.reset();
}