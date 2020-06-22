#pragma once

#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFGUI/Adjustment.hpp>
#include <SFGUI/Box.hpp>
#include <SFGUI/Button.hpp>
#include <SFGUI/CheckButton.hpp>
#include <SFGUI/ComboBox.hpp>
#include <SFGUI/Desktop.hpp>
#include <SFGUI/Label.hpp>
#include <SFGUI/Scale.hpp>
#include <SFGUI/Scrollbar.hpp>
#include <SFGUI/SFGUI.hpp>
#include <SFGUI/RadioButton.hpp>
#include <SFGUI/Window.hpp>

#include "Window.h"
#include "EventMgr.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Log.h"
#include "Lib.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "PolygonCollider.h"
#include "Random.h"

#define SCREENINDEX_NO_SCREEN -1

class IApp;

enum class ScreenState : unsigned char
{
    Running,
    ExitApp,
    ChangeNext,
    ChangePrevious,
    None,
    Count
};

class IScreen
{
public:
    friend class ScreenList;

    IScreen()
        : m_currentState(ScreenState::None),
          m_parentIApp(nullptr),
          m_screenIndex(SCREENINDEX_NO_SCREEN)
    {
    }
    virtual ~IScreen()
    {
    }

    virtual int GetNextScreenIndex() const = 0;
    virtual int GetPreviousScreenIndex() const = 0;

    virtual void Build() = 0;
    virtual void Destroy() = 0;

    virtual void OnEntry() = 0;
    virtual void OnExit() = 0;

    virtual void Update() = 0;
    virtual void Draw() = 0;

    int GetScreenIndex() const { return m_screenIndex; }

    void SetRunning() { m_currentState = ScreenState::Running; }

    ScreenState GetState() const { return m_currentState; }

    IApp *GetParentIApp() { return m_parentIApp; };
    void SetParentIApp(IApp *app) { m_parentIApp = app; }

protected:
    ScreenState m_currentState;
    IApp *m_parentIApp;
    int m_screenIndex;
};