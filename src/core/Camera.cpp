#include "Camera.h"

#include "LightningMgr.h"

sf::Transform Camera::m_transform = sf::Transform::Identity;
std::optional<const sf::Vector2f *> Camera::m_follow;

sf::Vector2f Camera::m_position(0.0f, 0.0f);
sf::Transform Camera::m_positionTransform = sf::Transform::Identity;

float Camera::m_rotation = 0.0f;
sf::Transform Camera::m_rotationTransform = sf::Transform::Identity;
float Camera::m_rps = 0.2f; // Rotations per second

sf::Vector2f Camera::m_zoom(1.0f, 1.0f);
sf::Transform Camera::m_zoomTransform = sf::Transform::Identity;

bool Camera::m_engaged = false;
sf::Vector2f Camera::m_lastPos(0.0f, 0.0f);

Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::Update()
{
    if (m_follow.has_value())
    {
        SetCenter(*m_follow.value());
    }
    else
    {
        const sf::Vector2f cur_pos = Mouse::GetPos();
        if (Mouse::IsDown(sf::Mouse::Button::Left) && Mouse::IsDown(sf::Mouse::Button::Right))
        {
            m_engaged = true;
            sf::Vector2f delta = cur_pos - m_lastPos;
            if (vl::LengthSq(delta) > 0.0f)
            {
                m_lastPos = cur_pos;
                delta = m_rotationTransform.getInverse().transformPoint(delta);
                delta = m_zoomTransform.getInverse().transformPoint(delta);
                delta *= -1.0f;
                Move(delta);
            }
        }
        else
        {
            m_lastPos = cur_pos;
            m_engaged = false;
        }
    }

    Zoom((Mouse::GetVerticalScroll() / 100.0f) + 1.0f);

    float angle = 0.0f;

    if (Keyboard::IsDown(sf::Keyboard::Q))
    {
        angle += m_rps * 360.0f * Clock::Delta().asSeconds();
    }
    if (Keyboard::IsDown(sf::Keyboard::E))
    {
        angle -= m_rps * 360.0f * Clock::Delta().asSeconds();
    }
    Rotate(angle);

    if (Keyboard::IsPressed(sf::Keyboard::R))
    {
        ResetTransformation();
    }
}

void Camera::Draw(const sf::Drawable &drawable, sf::RenderStates renderStates) noexcept
{
    renderStates.transform.combine(m_transform);
    Window::Render(drawable, renderStates);
}

void Camera::Draw(const sf::Drawable &drawable, sf::RenderTexture &texture, sf::RenderStates renderStates) noexcept
{
    renderStates.transform.combine(m_transform);
    texture.draw(drawable, renderStates);
}

void Camera::DrawText(const sf::Text &text, TextAlign align, sf::RenderStates renderStates) noexcept
{
    auto textCpy = text;
    float offset = 0.0f;
    switch (align)
    {
    case TextAlign::Left:
        break;
    case TextAlign::Middle:
        offset = textCpy.getLocalBounds().width / 2.0f;
        break;
    case TextAlign::Right:
        offset = textCpy.getLocalBounds().width;
        break;
    }

    textCpy.setPosition(text.getPosition().x - offset, text.getPosition().y);
    Camera::Draw(textCpy, renderStates);
}

void Camera::DrawPoint(const sf::Vector2f &position, sf::Color color, float radius) noexcept
{
    sf::CircleShape circle;
    float adjustedRadius = radius / m_zoom.x;
    circle.setPosition(position - sf::Vector2f(adjustedRadius, adjustedRadius));
    circle.setFillColor(color);
    circle.setRadius(adjustedRadius);
    Camera::Draw(circle);
}

void Camera::DrawRect(const sf::FloatRect rect, sf::Color fillColor, bool outlined, sf::Color outlineColor)
{
    sf::RectangleShape rectShape;
    rectShape.setPosition(rect.left, rect.top);
    rectShape.setSize(sf::Vector2f(rect.width, rect.height));
    rectShape.setFillColor(fillColor);
    if (outlined)
    {
        rectShape.setOutlineThickness(1.0f / m_zoom.x);
        rectShape.setOutlineColor(outlineColor);
    }
    Camera::Draw(rectShape);
}

void Camera::DrawLine(const sf::Vector2f &first, const sf::Vector2f &second, sf::Color color)
{
    sf::VertexArray line(sf::PrimitiveType::Lines, 2);
    line[0].color = color;
    line[1].color = color;
    line[0].position = first;
    line[1].position = second;
    Camera::Draw(line);
}

void Camera::Move(const sf::Vector2f &offset) noexcept
{
    SetCenter(m_position + offset);
}

void Camera::Zoom(float factor) noexcept
{
    m_zoom *= factor;
    m_zoomTransform.scale(factor, factor);
    UpdateTransform();
}

void Camera::Rotate(float angle) noexcept
{
    SetRotation(m_rotation + angle);
}

void Camera::SetCenter(const sf::Vector2f &center) noexcept
{
    m_position = center;
    m_positionTransform = sf::Transform().translate(m_position);
    UpdateTransform();
}

void Camera::SetZoom(float zoom) noexcept
{
    if (zoom != 0.0f)
    {
        LightningMgr::ChangeResolution(zoom / m_zoom.x);
        m_zoom = sf::Vector2f(zoom, zoom);
        m_zoomTransform = sf::Transform().scale(m_zoom, m_zoom);
        UpdateTransform();
    }
}

void Camera::SetRotation(float angle) noexcept
{
    m_rotation = angle;
    m_rotationTransform = sf::Transform().rotate(m_rotation);
    UpdateTransform();
}

sf::Vector2f Camera::ScreenToWorld(const sf::Vector2f &point) noexcept
{
    return m_transform.getInverse().transformPoint(point);
}

sf::FloatRect Camera::ScreenToWorld(const sf::FloatRect &rect) noexcept
{
    return m_transform.getInverse().transformRect(rect);
}

sf::Vector2f Camera::WorldToScreen(const sf::Vector2f &point) noexcept
{
    return m_transform.transformPoint(point);
}

sf::FloatRect Camera::WorldToScreen(const sf::FloatRect &rect) noexcept
{
    return m_transform.transformRect(rect);
}

void Camera::UpdateTransform() noexcept
{
    const sf::Vector2f offset(Lib::ConvertTo<float>(Window::GetSize()) / 2.0f);

    m_transform = sf::Transform::Identity;
    m_transform.translate(offset);
    m_transform.scale(m_zoom);
    m_transform.rotate(m_rotation);
    m_transform.translate(-m_position);
}

void Camera::CapZoomLevel() noexcept
{
    Lib::Constrain(m_zoom.x, 0.9f, 3.0f);
    Lib::Constrain(m_zoom.y, 0.9f, 3.0f);
}

void Camera::ResetTransformation() noexcept
{
    SetCenter(sf::Vector2f(0.0f, 0.0f));
    SetRotation(0.0f);
    SetZoom(1.0f);

    m_positionTransform = sf::Transform::Identity;
    m_rotationTransform = sf::Transform::Identity;
    m_zoomTransform = sf::Transform::Identity;
}