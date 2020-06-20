#include "LightningMgr.h"

std::vector<LightningMgr::RenderLight> LightningMgr::m_renderLights;

sf::Shader LightningMgr::m_shadowMapShader;
sf::Shader LightningMgr::m_renderLightsShader;
sf::Color LightningMgr::m_ambient(0, 0, 0);

sf::RenderTexture LightningMgr::m_collectiveLightMap;

float LightningMgr::m_resolution = 4096;

LightningMgr::LightningMgr()
{
    m_shadowMapShader.loadFromFile("res/shaders/vertexShader.vert", "res/shaders/shadowMap.frag");
    m_renderLightsShader.loadFromFile("res/shaders/vertexShader.vert", "res/shaders/renderLights.frag");
    m_collectiveLightMap.create(Window::GetSize().x, Window::GetSize().y);
}

LightningMgr::~LightningMgr()
{
    for (auto &renderLight : m_renderLights)
    {
        delete renderLight.occluders;
        delete renderLight.shadowMap;
        delete renderLight.lightMap;
        renderLight.occluders = nullptr;
        renderLight.shadowMap = nullptr;
        renderLight.lightMap = nullptr;
    }
}

void LightningMgr::Draw()
{
    Window::Draw(sf::Sprite(m_collectiveLightMap.getTexture()), sf::RenderStates(sf::BlendMultiply));
}

void LightningMgr::AddOccluder(const sf::Drawable &drawable, sf::FloatRect boundingBox, sf::RenderStates renderStates) noexcept
{
    sf::RenderStates copy = renderStates;
    for (auto &renderLight : m_renderLights)
    {
        if (boundingBox.intersects(renderLight.light->GetBoundingBox()))
        {
            float radius = renderLight.light->GetRadius();
            sf::Vector2f position = renderLight.light->GetPosition();
            copy.transform.combine(sf::Transform().translate(sf::Vector2f(radius, radius) - position));
            renderLight.occluders->draw(drawable, copy);
            copy = renderStates;
        }
    }
}

void LightningMgr::ClearOccluders() noexcept
{
    for (auto &renderLight : m_renderLights)
        renderLight.occluders->clear(sf::Color(0, 0, 0, 0));
}

void LightningMgr::DisplayOccluders() noexcept
{
    for (auto &renderLight : m_renderLights)
    {
        renderLight.occluders->display();
    }
}

void LightningMgr::Render() noexcept
{
    RenderShadowMaps();
    RenderLightMaps();
    RenderCollectiveLightMap();
}

void LightningMgr::AddLight(const Light::Ptr &light)
{
    m_renderLights.push_back(RenderLight(light));
}

void LightningMgr::RemoveLight(const Light::Ptr &light)
{
    auto findRes = std::find_if(m_renderLights.begin(), m_renderLights.end(), [&light](const RenderLight &renderLight) {
        return renderLight.light == light;
    });
    if (findRes != m_renderLights.end())
        m_renderLights.erase(findRes);
}

void LightningMgr::RenderShadowMaps() noexcept
{
    sf::FloatRect winRectAdjusted = Camera::ScreenToWorld(Lib::ConvertTo<float>(Window::GetScreenRect()));
    for (auto &renderLight : m_renderLights)
    {
        if (renderLight.light->GetBoundingBox().intersects(winRectAdjusted))
        {
            m_shadowMapShader.setUniform("resolution", m_resolution);
            renderLight.shadowMap->clear(sf::Color(0, 0, 0, 0));
            renderLight.occluders->draw(sf::Sprite(renderLight.light->GetStaticOccluderMap().getTexture()));
            renderLight.shadowMap->draw(sf::Sprite(renderLight.occluders->getTexture()), sf::RenderStates(&m_shadowMapShader));
            renderLight.shadowMap->display();
        }
    }
}

void LightningMgr::RenderLightMaps() noexcept
{
    sf::FloatRect winRectAdjusted = Camera::ScreenToWorld(Lib::ConvertTo<float>(Window::GetScreenRect()));
    for (auto &renderLight : m_renderLights)
    {
        if (renderLight.light->GetBoundingBox().intersects(winRectAdjusted))
        {
            m_renderLightsShader.setUniform("resolution", m_resolution);
            renderLight.lightMap->clear(sf::Color(0, 0, 0, 0));
            sf::Sprite shadowMapSprite(renderLight.shadowMap->getTexture());
            for (int i = 0; i < renderLight.light->GetBoundingBox().height; i++)
            {
                shadowMapSprite.setPosition(0.0, static_cast<float>(i));
                renderLight.lightMap->draw(shadowMapSprite, sf::RenderStates(&m_renderLightsShader));
            }
            renderLight.lightMap->display();
        }
    }
}

void LightningMgr::RenderCollectiveLightMap() noexcept
{
    sf::FloatRect winRectAdjusted = Camera::ScreenToWorld(Lib::ConvertTo<float>(Window::GetScreenRect()));
    m_collectiveLightMap.clear(sf::Color(m_ambient));
    for (auto &renderLight : m_renderLights)
    {
        if (renderLight.light->GetBoundingBox().intersects(winRectAdjusted))
        {
            sf::Sprite lightSprite(renderLight.lightMap->getTexture());
            sf::FloatRect box = renderLight.light->GetBoundingBox();
            lightSprite.setScale(box.width / lightSprite.getLocalBounds().width,
                                 box.height / lightSprite.getLocalBounds().height);
            lightSprite.setPosition(box.left, box.top);
            lightSprite.setColor(renderLight.light->GetColor());
            Camera::Draw(lightSprite, m_collectiveLightMap, sf::RenderStates(sf::BlendAdd));
        }
    }
    m_collectiveLightMap.display();
}