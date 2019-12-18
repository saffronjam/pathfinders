#include "Graphics.hpp"

#include "Math.hpp"

#include "Camera.hpp"

Graphics::Graphics(Camera &camera)
	: m_window(sf::VideoMode(Graphics::ScreenWidth, Graphics::ScreenHeight), "Application"),
	  m_transform(sf::Transform::Identity),
	  m_camera(camera)
{
	m_window.setVerticalSyncEnabled(true);

	allTextures.reserve(TextureMapping::TextureCount);
	allFonts.reserve(FontMapping::FontCount);

	// sf::Texture *GrassStandard = new sf::Texture;

	// GrassStandard->loadFromFile("assets/Grass.png");

	// allTextures.push_back(GrassStandard);

	sf::Font *FontArial = new sf::Font;

	FontArial->loadFromFile("assets/fonts/arial.ttf");

	allFonts.push_back(FontArial);
}

Graphics::~Graphics()
{
	for (auto texture : allTextures)
	{
		delete texture;
	}
	for (auto font : allFonts)
	{
		delete font;
	}
}

void Graphics::ClearFrame()
{
	m_window.clear();
}

void Graphics::EndFrame()
{
	m_window.display();
	m_independantPreTransform = sf::Transform::Identity;
	m_independantPostTransform = sf::Transform::Identity;
}

void Graphics::Render(const sf::Drawable &drawable)
{
	m_transform = m_independantPreTransform * m_transform;
	m_transform = m_transform * m_independantPostTransform;
	m_window.draw(drawable, m_transform);
	m_transform = sf::Transform::Identity;
}

void Graphics::Render(sf::Vector2f point, float radius, sf::Color color)
{
	sf::CircleShape dot(radius);
	dot.setFillColor(color);
	dot.setPosition({point.x - radius, point.y - radius});
	m_camera.PushChain(dot);
}

void Graphics::Render(const sf::FloatRect rect, sf::Color color, bool outlined)
{
	sf::RectangleShape toDraw = sf::RectangleShape(sf::Vector2f(rect.width, rect.height));
	toDraw.setPosition(sf::Vector2f(rect.left, rect.top));
	if (outlined)
		toDraw.setFillColor(sf::Color::Transparent);
	else
		toDraw.setFillColor(color);

	toDraw.setOutlineColor(color);
	toDraw.setOutlineThickness(1.0f);
	Draw(toDraw);
}

void Graphics::Render(const sf::IntRect rect, sf::Color color, bool outlined)
{
	Draw(sf::FloatRect(rect), color, outlined);
}

void Graphics::RenderLine(sf::Vector2f point1, sf::Vector2f point2, sf::Color color)
{
	sf::VertexArray lines(sf::Lines, 2);
	lines[0].position = point1;
	lines[0].color = color;
	lines[1].position = point2;
	lines[1].color = color;
	Render(lines);
}

void Graphics::RenderLine(float x1, float y1, float x2, float y2, sf::Color color)
{
	RenderLine(sf::Vector2f(x1, y1), sf::Vector2f(x2, y2), color);
}

void Graphics::Draw(const sf::Drawable &drawable)
{
	m_camera.PushChain(drawable);
}

void Graphics::Draw(sf::Vector2f point, float radius, sf::Color color)
{
	sf::CircleShape dot(radius);
	dot.setFillColor(color);
	dot.setPosition({point.x - radius, point.y - radius});
	m_camera.PushChain(dot);
}

void Graphics::Draw(const sf::FloatRect rect, sf::Color color, bool outlined)
{
	sf::RectangleShape toDraw = sf::RectangleShape(sf::Vector2f(rect.width, rect.height));
	toDraw.setPosition(sf::Vector2f(rect.left, rect.top));
	if (outlined)
		toDraw.setFillColor(sf::Color::Transparent);
	else
		toDraw.setFillColor(color);

	toDraw.setOutlineColor(color);
	toDraw.setOutlineThickness(1.0f);
	Draw(toDraw);
}

void Graphics::Draw(const sf::IntRect rect, sf::Color color, bool outlined)
{
	Draw(sf::FloatRect(rect), color, outlined);
}

void Graphics::DrawLine(sf::Vector2f point1, sf::Vector2f point2, sf::Color color)
{
	sf::VertexArray lines(sf::Lines, 2);
	lines[0].position = point1;
	lines[0].color = color;
	lines[1].position = point2;
	lines[1].color = color;
	m_camera.PushChain(lines);
}

void Graphics::DrawLine(float x1, float y1, float x2, float y2, sf::Color color)
{
	DrawLine(sf::Vector2f(x1, y1), sf::Vector2f(x2, y2), color);
}

void Graphics::ApplyTransformation(sf::Transform add)
{
	m_transform = m_transform * add;
}

void Graphics::ApplyIndependantPreTransformation(sf::Transform add)
{
	m_independantPreTransform = m_independantPreTransform * add;
}

void Graphics::ApplyIndependantPostTransformation(sf::Transform add)
{
	m_independantPostTransform = m_independantPostTransform * add;
}

sf::Vector2f Graphics::AddDefaultTranslation(sf::Vector2f vector)
{
	vector -= (sf::Vector2f)m_camera.GetOffset();
	vector += m_camera.GetPos();
	return vector;
}

sf::Vector2f Graphics::AddDefaultRotation(sf::Vector2f vector)
{
	const float cosTheta = cos((-m_camera.GetAngle() * Math::Constants::PI) / 180);
	const float sinTheta = sin((-m_camera.GetAngle() * Math::Constants::PI) / 180);
	const float new_x = vector.x * cosTheta - vector.y * sinTheta;
	vector.y = vector.x * sinTheta + vector.y * cosTheta;
	vector.x = new_x;
	return vector;
}

sf::Vector2f Graphics::AddDefaultScale(sf::Vector2f vector)
{
	vector.x /= m_camera.GetZoom().x;
	vector.y /= m_camera.GetZoom().y;
	return vector;
}