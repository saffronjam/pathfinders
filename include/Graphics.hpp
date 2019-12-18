#pragma once
#include <SFML/Graphics.hpp>

#include "Funclib.hpp"

class Graphics
{
public:
	enum TextureMapping
	{
		TextureCount
	};
	enum FontMapping
	{
		FontArial,
		FontCount
	};

public:
	Graphics(class Camera &camera);
	~Graphics();
	void ClearFrame();
	void EndFrame();

	void Render(const sf::Drawable &drawable);
	void Render(sf::Vector2f point, float radius = 1.0f, sf::Color color = sf::Color::White);
	void Render(const sf::FloatRect rect, sf::Color color = sf::Color::White, bool outlined = false);
	void Render(const sf::IntRect rect, sf::Color color = sf::Color::White, bool outlined = false);
	void RenderLine(sf::Vector2f point1, sf::Vector2f point2, sf::Color color = sf::Color::White);
	void RenderLine(float x1, float y1, float x2, float y2, sf::Color color = sf::Color::White);

	void Draw(const sf::Drawable &drawable);
	void Draw(sf::Vector2f point, float radius = 1.0f, sf::Color color = sf::Color::White);
	void Draw(const sf::FloatRect rect, sf::Color color = sf::Color::White, bool outlined = false);
	void Draw(const sf::IntRect rect, sf::Color color = sf::Color::White, bool outlined = false);

	void DrawLine(sf::Vector2f point1, sf::Vector2f point2, sf::Color color = sf::Color::White);
	void DrawLine(float x1, float y1, float x2, float y2, sf::Color color = sf::Color::White);

	void ApplyTransformation(sf::Transform add);
	void ApplyIndependantPreTransformation(sf::Transform add);
	void ApplyIndependantPostTransformation(sf::Transform add);

	sf::Vector2f AddDefaultTranslation(sf::Vector2f vector);
	sf::Vector2f AddDefaultRotation(sf::Vector2f vector);
	sf::Vector2f AddDefaultScale(sf::Vector2f vector);

	sf::RenderWindow &GetRenderWindow() { return m_window; }

	std::vector<sf::Texture *> allTextures;
	std::vector<sf::Font *> allFonts;

private:
	sf::RenderWindow m_window;
	sf::Transform m_transform;
	sf::Transform m_independantPreTransform;
	sf::Transform m_independantPostTransform;

	class Camera &m_camera;

public:
	static constexpr int ScreenWidth = 1000;
	static constexpr int ScreenHeight = 600;
	static sf::Vector2f HalfScreen() { return sf::Vector2f((float)ScreenWidth / 2.0f, (float)ScreenHeight / 2.0f); }
	static sf::IntRect ScreenRect() { return sf::IntRect(0, 0, ScreenWidth, ScreenHeight); }
};
