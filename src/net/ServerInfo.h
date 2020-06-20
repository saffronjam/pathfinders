#pragma once

#include "IConnInfo.h"

class ServerInfo : public IConnInfo
{
public:
    ServerInfo()
        : IConnInfo(1)
    {
    }
};