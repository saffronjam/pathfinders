#include "IException.h"

IException::IException(int line, const char *file) noexcept
    : m_line(line), m_file(file)
{
}

const char *IException::what() const noexcept
{
    std::ostringstream oss;
    oss << "[Type] " << GetType() << std::endl
        << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char *IException::GetType() const noexcept
{
    return "IException";
}

std::string IException::GetOriginString() const noexcept
{
    std::ostringstream oss;
    oss << "[File] " << m_file << std::endl
        << "[Line] " << m_line << std::endl;
    return oss.str();
}
