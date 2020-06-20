#pragma once

#include <memory>

#include <SFML/Audio/Music.hpp>

#include <IResourceMgr.h>

class MusicMgr : public IResourceMgr<std::shared_ptr<sf::Music>>
{
public:
    MusicMgr() = default;
    MusicMgr(const MusicMgr &) = delete;
    const MusicMgr &operator()(const MusicMgr &) = delete;

public:
    // Returns copy of resource from cache, if not existing, call Load();
    static const std::shared_ptr<sf::Music> &GetCopy(const std::string &filepath) noexcept
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
        std::shared_ptr<sf::Music> resource = std::make_shared<sf::Music>();
        if (!resource->openFromFile(filepath))
        {
            THROW(Exception, "Failed to open music: %s", filepath.c_str());
        }
        m_resources.emplace(std::make_pair(filepath, resource));
    }
};