#pragma once

#include <set>
#include <mutex>
#include <algorithm>

#include "IPacketHandler.h"
#include "ParsedPacket.h"

class PacketMgr
{
public:
    static void AddPacketBuffer(PacketBuffer *buffer, std::mutex *mutex);
    static void AddHandler(IPacketHandler *handler);

    static void RemovePacketBuffer(PacketBuffer *buffer);
    static void RemoveHandler(IPacketHandler *handler);

    static void HandleAllPackets();

private:
    static std::set<std::pair<PacketBuffer *, std::mutex *>> m_packetBuffers;
    static std::set<IPacketHandler *> m_handlers;
};
