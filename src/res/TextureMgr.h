#pragma once

#include <SFML/Graphics/Texture.hpp>

#include "IResourceMgr.h"
#include "GenericThrowMacros.h"

class TextureMgr : public IResourceMgr<sf::Texture>
{
public:
    TextureMgr() = default;
    TextureMgr(const TextureMgr &) = delete;
    const TextureMgr &operator()(const TextureMgr &) = delete;

    // Returns pointer resource from cache, if not existing, call Load();
    static sf::Texture *Get(const std::string &filepath) noexcept
    {
        if (m_resources.find(filepath) == m_resources.end())
        {
            Load(filepath);
        }
        return &m_resources[filepath];
    }
    // Returns copy of resource from cache, if not existing, call Load();
    static const sf::Texture &GetCopy(const std::string &filepath) noexcept
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
        sf::Texture resource;
        if (!resource.loadFromFile(filepath))
        {
            THROW(Exception, "Failed to load texture: %s", filepath.c_str());
        }
        m_resources.emplace(std::make_pair(filepath, resource));
    }
};