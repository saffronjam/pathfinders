#include "ScreenList.h"

#include "IScreen.h"

ScreenList::ScreenList(IApp *app)
    : m_app(app),
      m_currentScreenIndex(SCREENINDEX_NO_SCREEN)
{
}

ScreenList::~ScreenList()
{
    m_currentScreenIndex = SCREENINDEX_NO_SCREEN;
}

IScreen *ScreenList::MoveNext()
{
    IScreen *currentScreen = GetCurrent();
    if (currentScreen->GetNextScreenIndex() != SCREENINDEX_NO_SCREEN)
        m_currentScreenIndex = currentScreen->GetNextScreenIndex();
    return GetCurrent();
}

IScreen *ScreenList::MovePrevious()
{
    IScreen *currentScreen = GetCurrent();
    if (currentScreen->GetPreviousScreenIndex() != SCREENINDEX_NO_SCREEN)
        m_currentScreenIndex = currentScreen->GetPreviousScreenIndex();
    return GetCurrent();
}

void ScreenList::SetCurrentScreen(int nextScreen)
{
    m_currentScreenIndex = nextScreen;
}

void ScreenList::AddScreen(IScreen *screen)
{
    screen->m_screenIndex = m_screens.size();
    m_screens.push_back(screen);
    screen->Build();
    screen->SetParentIApp(m_app);
}

IScreen *ScreenList::GetCurrent()
{
    return m_currentScreenIndex == SCREENINDEX_NO_SCREEN ? nullptr : m_screens[m_currentScreenIndex];
}