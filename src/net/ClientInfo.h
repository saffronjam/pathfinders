#pragma once

#include "IConnInfo.h"

class ClientInfo : public IConnInfo
{
public:
    ClientInfo(sf::Uint64 uid)
        : IConnInfo(uid)
    {
    }
};