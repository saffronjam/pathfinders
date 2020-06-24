#pragma once

#include "IScreen.h"

#include "PathfinderMgr.h"

class AppClient;

class ClientMainScreen : public IScreen
{
public:
    ClientMainScreen(AppClient &parent);
    ~ClientMainScreen();

    virtual void Build() override;

    virtual void Destroy() override;

    virtual void OnEntry() override;

    virtual void OnExit() override;

    virtual void Update() override;

    virtual void Draw() override;

    virtual int GetNextScreenIndex() const override;

    virtual int GetPreviousScreenIndex() const override;

private:
    void CheckInput();

private:
    AppClient &m_parent;

    PathfinderMgr m_pathfinderMgr;

    Voronoi voronoi;
};