#pragma once

#include <vector>

#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

#include "PointLight.h"
#include "VectorLib.h"
#include "Camera.h"

class LightningMgr
{
private:
    struct RenderLight
    {
        RenderLight(const Light::Ptr &light)
            : light(light),
              occluders(new sf::RenderTexture()),
              shadowMap(new sf::RenderTexture()),
              lightMap(new sf::RenderTexture())
        {
            auto box = light->GetBoundingBox();
            occluders->create(box.width, box.height);
            shadowMap->create(box.width, 1);
            lightMap->create(box.width, box.height);
        }
        Light::Ptr light;
        sf::RenderTexture *occluders;
        sf::RenderTexture *shadowMap;
        sf::RenderTexture *lightMap;
    };

public:
    LightningMgr();
    ~LightningMgr();
    LightningMgr(LightningMgr &) = delete;
    LightningMgr &operator()(LightningMgr &) = delete;

    static void Draw();

    static void AddOccluder(const sf::Drawable &drawable, sf::FloatRect boundingBox, sf::RenderStates renderStates = sf::RenderStates::Default) noexcept;
    static void ClearOccluders() noexcept;
    static void DisplayOccluders() noexcept;

    static void Render() noexcept;

    static void AddLight(const Light::Ptr &light);
    static void RemoveLight(const Light::Ptr &light);
    static void SetAmbientLight(const sf::Color &ambient) noexcept { m_ambient = ambient; }

    static void ChangeResolution(float factor) noexcept { m_resolution *= factor; }

    static float GetResolution() noexcept { return m_resolution; }
    static void SetResolution(float resolution) noexcept { m_resolution = resolution; }

private:
    static void RenderShadowMaps() noexcept;
    static void RenderLightMaps() noexcept;
    static void RenderCollectiveLightMap() noexcept;

private:
    static std::vector<RenderLight> m_renderLights;
    static sf::Shader m_shadowMapShader;
    static sf::Shader m_renderLightsShader;
    static sf::RenderTexture m_collectiveLightMap;
    static sf::Color m_ambient;

    static float m_resolution;
};