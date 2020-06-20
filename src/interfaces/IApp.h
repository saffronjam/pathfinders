#pragma once

#include <memory>

#include "AppThrowMacros.h"
#include "Window.h"
#include "Camera.h"
#include "EventMgr.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "GuiMgr.h"
#include "ScreenList.h"
#include "TextureMgr.h"
#include "FontMgr.h"
#include "MusicMgr.h"
#include "SoundBufferMgr.h"
#include "LightningMgr.h"
#include "PhysicsMgr.h"
#include "PacketMgr.h"
#include "IScreen.h"
#include "Clock.h"
#include "FPSLimiter.h"

class IApp : public EventHandler
{
public:
    IApp();
    virtual ~IApp();

    void Run();
    void Exit();

    virtual void OnInit() = 0;
    virtual void OnExit() = 0;
    virtual void OnPreUpdate() = 0;
    virtual void OnPostUpdate() = 0;
    virtual void AddScreens() = 0;

    TextureMgr &GetTextureMgr() { return m_textureMgr; };
    FontMgr &GetFontMgr() { return m_fontMgr; };
    MusicMgr &GetMusicMgr() { return m_musicMgr; };
    SoundBufferMgr &GetSoundBufferMgr() { return m_soundBufferMgr; };

protected:
    virtual void Update();
    virtual void Draw();

    bool Init();

    virtual void OnEvent(const sf::Event &event) override;

protected:
    sf::VideoMode m_videoMode;
    Window m_window;
    Camera m_camera;
    LightningMgr m_lightningMgr;
    PhysicsMgr m_physicsMgr;

    EventMgr m_eventMgr;
    Keyboard m_keyboard;
    Mouse m_mouse;
    GuiMgr m_guiMgr;
    Clock m_clock;
    FPSLimiter m_fpsLimiter;

    TextureMgr m_textureMgr;
    FontMgr m_fontMgr;
    MusicMgr m_musicMgr;
    SoundBufferMgr m_soundBufferMgr;

    std::unique_ptr<ScreenList> m_screenList;
    IScreen *m_currentScreen;

private:
    bool m_isRunning;

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