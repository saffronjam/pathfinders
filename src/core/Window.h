#pragma once

#include <string>
#include <cassert>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Err.hpp>

#include "WindowThrowMacros.h"
#include "IException.h"
#include "TextAlign.h"
#include "Log.h"
#include "Lib.h"

class Window
{
    friend class Draw;
    friend class Camera;

public:
    Window(const std::string &title, int width, int height);
    ~Window();
    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    static void Draw(const sf::Drawable &drawable, sf::RenderStates renderStates = sf::RenderStates::Default) noexcept;
    static void DrawText(const sf::Text &text, TextAlign align = TextAlign::Left, sf::RenderStates renderStates = sf::RenderStates::Default) noexcept;
    static void DrawPoint(const sf::Vector2f &position, sf::Color color = sf::Color::Red, float radius = 3.0f) noexcept;
    static void DrawRect(const sf::FloatRect rect, sf::Color fillColor = sf::Color::Red, bool outlined = false, sf::Color outlineColor = sf::Color::Black);
    static void DrawLine(const sf::Vector2f &first, const sf::Vector2f &second, sf::Color color = sf::Color::Red);
    static void Clear();
    static void Present() noexcept;

    static void PositionCenter() noexcept;

    static sf::RenderWindow *GetSFWindow() noexcept;
    static sf::Vector2i GetPosition() noexcept;
    static sf::Vector2u GetSize() noexcept;
    static int GetWidth() noexcept;
    static int GetHeight() noexcept;
    static const std::string &GetTitle() noexcept;
    static sf::View GetCurrentView() noexcept;
    static sf::View GetDefaultView() noexcept;
    static sf::IntRect GetScreenRect() noexcept;

    static bool IsFullscreen() noexcept;
    static bool IsVSyncEnabled() noexcept;

    static void SetPosition(const sf::Vector2i &pos) noexcept;
    static void SetSize(const sf::Vector2u &size) noexcept;
    static void SetTitle(const std::string &title) noexcept;
    static void SetIcon(const std::string &icon) noexcept;
    static void SetFullscreen(bool toggle) noexcept;
    static void SetVSync(bool toggle) noexcept;

private:
    static void Render(const sf::Drawable &drawable, sf::RenderStates renderStates = sf::RenderStates::Default) noexcept;
    static void ResetNdcTransform() noexcept;

private:
    static sf::RenderWindow *m_sfWindow;
    static std::string m_title;
    // Used after exiting fullscreen
    static sf::VideoMode m_videomode;
    // Used after exiting fullscreen
    static sf::Uint32 m_style;
    static sf::Vector2i m_nonFullscreenPosition;

    static bool m_fullscreen;

public:
    class Exception : public IException
    {
    public:
        Exception(int line, const char *file, const char *errorString) noexcept;
        const char *what() const noexcept override;
        virtual const char *GetType() const noexcept override;
        const char *GetErrorString() const noexcept;

    private:
        std::string errorString;
    };
};