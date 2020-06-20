#pragma once

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>

#include <IResourceMgr.h>

class SoundBufferMgr : public IResourceMgr<sf::SoundBuffer>
{
public:
    SoundBufferMgr() = default;
    SoundBufferMgr(const SoundBufferMgr &) = delete;
    const SoundBufferMgr &operator()(const SoundBufferMgr &) = delete;

    // Returns pointer resource from cache, if not existing, call Load();
    static sf::SoundBuffer *Get(const std::string &filepath) noexcept
    {
        if (m_resources.find(filepath) == m_resources.end())
        {
            Load(filepath);
        }
        return &m_resources[filepath];
    }
    // Returns copy of resource from cache, if not existing, call Load();
    static const sf::SoundBuffer &GetCopy(const std::string &filepath) noexcept
    {
        if (m_resources.find(filepath) == m_resources.end())
        {
            Load(filepath);
        }
        return m_resources[filepath];
    }
    // Load resource into memory
    static void Load(const std::string &filepath)
    {
        sf::SoundBuffer resource;
        if (!resource.loadFromFile(filepath))
        {
            THROW(Exception, "Failed to load soundbuffer: %s", filepath.c_str());
        }
        m_resources.emplace(std::make_pair(filepath, resource));
    }
};