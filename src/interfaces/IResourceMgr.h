#pragma once

#include <map>

#include "IException.h"
#include "GenericThrowMacros.h"

template <class T>
class IResourceMgr
{
public:
    IResourceMgr() = default;

protected:
    static std::map<std::string, T> m_resources;

public:
    class Exception : public IException
    {
    public:
        Exception(int line, const char *file, const char *errorString) noexcept
            : IException(line, file),
              errorString(errorString)
        {
        }
        const char *what() const noexcept override
        {
            std::ostringstream oss;
            oss << "[Type] " << GetType() << std::endl
                << "[Description] " << GetErrorString() << std::endl
                << GetOriginString();
            whatBuffer = oss.str();
            return whatBuffer.c_str();
        }
        virtual const char *GetType() const noexcept override
        {
            return "V-2DEngine ResourceMgr Exception";
        }
        const char *GetErrorString() const noexcept
        {
            return errorString.c_str();
        }

    private:
        std::string errorString;
    };
};

template <typename T>
std::map<std::string, T> IResourceMgr<T>::m_resources;