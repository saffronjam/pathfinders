#pragma once

#pragma once

#include <vector>

class IApp;
class IScreen;

class ScreenList
{
public:
    ScreenList(IApp *app);
    ~ScreenList();

    IScreen *MoveNext();
    IScreen *MovePrevious();

    void SetCurrentScreen(int nextScreen);
    void AddScreen(IScreen *screen);

    IScreen *GetCurrent();

protected:
    IApp *m_app;
    std::vector<IScreen *> m_screens;
    int m_currentScreenIndex;
};