#pragma once

#include "ParsedPacket.h"
#include "Connection.h"

#define HandlePacket(type, packet) Handle##type##Packet(packet)

class IPacketHandler
{
    friend class PacketMgr;

protected:
    virtual void HandlePacket(AreYouAlive, const ParsedPacket &packet) {}
    virtual void HandlePacket(IAmAlive, const ParsedPacket &packet) {}
    virtual void HandlePacket(UID, const ParsedPacket &packet) {}
    virtual void HandlePacket(UDPSync, const ParsedPacket &packet) {}
};