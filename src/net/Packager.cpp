#include "Packager.h"

sf::Packet Packager::Pack(PacketType type, sf::Uint64 id, const sf::Uint8 *data, size_t size)
{
    sf::Packet packet;
    packet.append(&type, sizeof(PacketType));
    packet.append(&id, sizeof(sf::Uint64));
    if (data)
        packet.append(data, size);
    return packet;
}

std::optional<PacketType> Packager::GetTypeFromRawPacket(const sf::Packet &rawPacket)
{
    try
    {
        if (!rawPacket.getData())
            THROW(Exception, "Tried to fetch type from packet without any data: Pointer: %p, Packet size: %zu", rawPacket.getData(), rawPacket.getDataSize());

        PacketType type = *reinterpret_cast<const PacketType *>(rawPacket.getData());
        if (type < PacketType::Invalid || type > PacketType::Count)
            THROW(Exception, "Fetch invalid type from packet. Type: %u", type);
        return type;
    }
    catch (const IException &e)
    {
#ifdef LOG_EXCEPTION
        LogWhat;
#endif
        return std::nullopt;
    }
    catch (const std::exception &e)
    {
#ifdef LOG_EXCEPTION
        LogWhat;
#endif
        return std::nullopt;
    }
    catch (...)
    {
#ifdef LOG_EXCEPTION
        LogNoDetails;
#endif
        return std::nullopt;
    }
}

std::optional<sf::Uint64> Packager::GetUIDFromRawPacket(const sf::Packet &rawPacket)
{
    try
    {
        if (!rawPacket.getData())
            THROW(Exception, "Tried to fetch UID from packet without any data: Pointer: %p, Packet size: %zu", rawPacket.getData(), rawPacket.getDataSize());

        const sf::Uint8 *cData = reinterpret_cast<const sf::Uint8 *>(rawPacket.getData());
        sf::Uint64 uid = *reinterpret_cast<const sf::Uint64 *>(cData + sizeof(PacketType));
        return uid;
    }
    catch (const IException &e)
    {
#ifdef LOG_EXCEPTION
        LogWhat;
#endif
        return std::nullopt;
    }
    catch (const std::exception &e)
    {
#ifdef LOG_EXCEPTION
        LogWhat;
#endif
        return std::nullopt;
    }
    catch (...)
    {
#ifdef LOG_EXCEPTION
        LogNoDetails;
#endif
        return std::nullopt;
    }
}

Packager::Exception::Exception(int line, const char *file, const char *errorString) noexcept
    : IException(line, file),
      errorString(errorString)
{
}

const char *Packager::Exception::what() const noexcept
{
    std::ostringstream oss;
    oss << "[Type] " << GetType() << std::endl
        << "[Description] " << GetErrorString() << std::endl
        << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}
const char *Packager::Exception::GetType() const noexcept
{
    return "V-2DEngine Packager Exception";
}
const char *Packager::Exception::GetErrorString() const noexcept
{
    return errorString.c_str();
}