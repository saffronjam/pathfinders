#include "INetModule.h"

#include "INetMgr.h"

INetModule::INetModule(INetMgr *ioHelper)
    : m_ioHelper(ioHelper)
{
}