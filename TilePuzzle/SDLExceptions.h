#pragma once

#include <exception>
#include <string>

class SDLException : public std::exception
{
public:
    explicit SDLException(const char* message) : m_errorMessage(message) {}

    [[nodiscard]] const char* what() const noexcept override
    {
        return m_errorMessage.c_str();
    }

private:
    std::string m_errorMessage;
};

class SDLInitException final : public SDLException
{
public:
    using SDLException::SDLException;
};

class SDLImageLoadException final : public SDLException
{
public:
    using SDLException::SDLException;
};