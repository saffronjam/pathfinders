#pragma once

#include <cstring>
#include <array>
#include <memory>

#include <SFML/Network/Packet.hpp>

#include "IException.h"
#include "Connection.h"
#include "ParsedPacket.h"
#include "PacketType.h"
#include "Protocol.h"
#include "Log.h"
#include "GenericThrowMacros.h"
#include "Lib.h"

class Packager
{
public:
    static sf::Packet Pack(PacketType type, sf::Uint64 id, const sf::Uint8 *data, size_t size);
    template <Protocol P>
    static std::optional<ParsedPacket> Parse(const sf::Packet &packet, const Connection *conn, IConnInfo *connInf);

    static std::optional<PacketType> GetTypeFromRawPacket(const sf::Packet &rawPacket);
    static std::optional<sf::Uint64> GetUIDFromRawPacket(const sf::Packet &rawPacket);

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

template <Protocol P>
std::optional<ParsedPacket> Packager::Parse(const sf::Packet &packet, const Connection *conn, IConnInfo *connInfo)
{
    try
    {
        sf::Uint8 *rawPacketData = (sf::Uint8 *)packet.getData();

        ParsedPacket parsedPacket;

        const int header = sizeof(PacketType) + sizeof(sf::Uint64);

        // CHECK SIZES
        parsedPacket.fullSize = packet.getDataSize();
        parsedPacket.dataSize = parsedPacket.fullSize - header;
        if (parsedPacket.dataSize < 0)
        {
            THROW(Exception, "Data size was too small to be a valid packet. Data size: %zu", parsedPacket.dataSize);
        }

        // TYPE
        auto typeOpt = GetTypeFromRawPacket(packet);
        if (!typeOpt.has_value())
            THROW(Exception, "No valid type was found in packet. Data size: %zu", parsedPacket.dataSize);
        parsedPacket.type = typeOpt.value();

        // COPY DATA IF THERE IS ANY
        if (parsedPacket.dataSize)
        {
            parsedPacket.data = std::shared_ptr<sf::Uint8>(new sf::Uint8[parsedPacket.dataSize]);
            memcpy(parsedPacket.data.get(), rawPacketData + header, parsedPacket.dataSize);
        }

        if (!conn || !connInfo)
            THROW(Exception, "Either Connection pointer or ConnInfo pointer was null: Connection: %p, ConnInfo: %p", conn, connInfo);
        parsedPacket.connection = conn;
        parsedPacket.connInfo = connInfo;
        parsedPacket.protocol = P;

        return parsedPacket;
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
