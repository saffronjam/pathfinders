#include "SetupModule.h"

#include "INetMgr.h"

SetupModule::SetupModule(INetMgr *ioHelper)
    : INetModule(ioHelper)
{
}

void SetupModule::HandlePacket(UID, const ParsedPacket &packet)
{
    sf::Uint64 uid = *reinterpret_cast<sf::Uint64 *>(packet.data.get());
    m_ioHelper->SetUID(uid);
    m_ioHelper->SendEmpty<UDP>(UDPSync, packet.connection);
}

void SetupModule::HandlePacket(UDPSync, const ParsedPacket &packet)
{
}
