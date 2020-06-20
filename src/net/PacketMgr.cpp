#include "PacketMgr.h"

std::set<std::pair<PacketBuffer *, std::mutex *>> PacketMgr::m_packetBuffers;
std::set<IPacketHandler *> PacketMgr::m_handlers;

void PacketMgr::AddPacketBuffer(PacketBuffer *buffer, std::mutex *mutex)
{
    m_packetBuffers.emplace(std::make_pair(buffer, mutex));
}

void PacketMgr::AddHandler(IPacketHandler *handler)
{
    m_handlers.emplace(handler);
}

void PacketMgr::RemovePacketBuffer(PacketBuffer *buffer)
{
    for (auto &pair : m_packetBuffers)
    {
        if (pair.first == buffer)
        {
            m_packetBuffers.erase(pair);
            return;
        }
    }
}

void PacketMgr::RemoveHandler(IPacketHandler *handler)
{
    m_handlers.erase(handler);
}

void PacketMgr::HandleAllPackets()
{
    for (auto &[packetBuffer, mutex] : m_packetBuffers)
    {
        mutex->lock();
        for (auto &packet : *packetBuffer)
        {
            for (auto &handler : m_handlers)
            {
                switch (packet.type)
                {
                case AreYouAlive:
                    handler->HandlePacket(AreYouAlive, packet);
                    break;
                case IAmAlive:
                    handler->HandlePacket(IAmAlive, packet);
                    break;
                case UID:
                    handler->HandlePacket(UID, packet);
                    break;
                case UDPSync:
                    handler->HandlePacket(UDPSync, packet);
                    break;
                }
            }
        }
        packetBuffer->clear();
        mutex->unlock();
    }
}