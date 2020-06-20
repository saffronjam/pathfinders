#pragma once

#include <vector>
#include <memory>

#include <SFML/Config.hpp>

#include "PacketType.h"
#include "Protocol.h"
#include "Connection.h"
#include "IConnInfo.h"

struct ParsedPacket
{
    PacketType type = PacketType::Invalid;
    std::shared_ptr<sf::Uint8> data;
    size_t dataSize = 0;
    size_t fullSize = 0;
    const Connection *connection = nullptr;
    IConnInfo *connInfo = nullptr;
    Protocol protocol = Protocol::None;
};

using PacketBuffer = std::vector<ParsedPacket>;