#include "ClientMainScreen.h"
#include "AppClient.h"

ClientMainScreen::ClientMainScreen(AppClient &parent)
    : m_parent(parent)
{
}

ClientMainScreen::~ClientMainScreen()
{
}

void ClientMainScreen::Build()
{
}

void ClientMainScreen::Destroy()
{
}

void ClientMainScreen::OnEntry()
{

    // -------------- ALL LABELS ------------------
    auto labelGrid = sfg::Label::Create("Grid");
    auto labelAlgorithms = sfg::Label::Create("Algorithms");
    auto labelTools = sfg::Label::Create("Tools");
    auto labelSettings = sfg::Label::Create("Settings");

    // -------------- SLEEP DELAY ------------------
    auto sleepDelayLabel = sfg::Label::Create();
    auto sleepDelayScale = sfg::Scale::Create(sfg::Scale::Orientation::HORIZONTAL);

    auto sleepDelayAdjustment = sleepDelayScale->GetAdjustment();

    sleepDelayAdjustment->SetLower(0.0f);
    sleepDelayAdjustment->SetUpper(26.826957952797f);
    sleepDelayAdjustment->SetValue(13.894954943731f);

    sleepDelayAdjustment->SetMinorStep(0.1f);
    sleepDelayAdjustment->SetMajorStep(0.3f);

    sleepDelayAdjustment->GetSignal(sfg::Adjustment::OnChange).Connect([sleepDelayAdjustment, sleepDelayLabel, this] {
        auto valMul = std::pow(sleepDelayAdjustment->GetValue(), 3.5f);
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(0) << "Sleep delay: " << valMul << " us";
        sleepDelayLabel->SetText(oss.str());
        m_pathfinderMgr.SetSleepDelay(sf::microseconds(valMul));
    });
    {
        auto valMul = std::pow(sleepDelayAdjustment->GetValue(), 3.5f);
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(0) << "Sleep delay: " << valMul << " us";
        sleepDelayLabel->SetText(oss.str());
        m_pathfinderMgr.SetSleepDelay(sf::microseconds(valMul));
    }

    sleepDelayScale->SetRequisition(sf::Vector2f(80.f, 20.f));

    auto sleepDelayScalebox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
    sleepDelayScalebox->Pack(sleepDelayLabel, false, false);
    sleepDelayScalebox->Pack(sleepDelayScale, false, false);
    sleepDelayScalebox->SetRequisition(sf::Vector2f(150.0f, 0.0f));

    auto sleepDelayCheck = sfg::CheckButton::Create("");
    sleepDelayCheck->SetActive(true);
    sleepDelayCheck->GetSignal(sfg::ToggleButton::OnToggle).Connect([sleepDelayCheck, sleepDelayLabel, sleepDelayAdjustment, sleepDelayScale, this] {
        if (sleepDelayCheck->IsActive())
        {
            sleepDelayScale->SetState(sfg::Widget::State::NORMAL);
            auto valMul = std::pow(sleepDelayAdjustment->GetValue(), 1.5f);
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(0) << "Sleep delay " << valMul << " us";
            sleepDelayLabel->SetText(oss.str());
            m_pathfinderMgr.SetSleepDelay(sf::microseconds(valMul));
        }
        else
        {
            sleepDelayScale->SetState(sfg::Widget::State::INSENSITIVE);
            sleepDelayLabel->SetText("Sleep delay deactivated");
            m_pathfinderMgr.SetSleepDelay(sf::microseconds(0));
        }
    });

    auto sleepDelaySettingsBox = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 5.0f);
    sleepDelaySettingsBox->Pack(sleepDelayCheck, false);
    sleepDelaySettingsBox->Pack(sleepDelayScalebox, false);

    // -------------- PACK SCALE IN BOX ------------------
    auto scaleBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 10.0f);
    scaleBox->Pack(sleepDelaySettingsBox);

    // -------------- ACTIVE ALGORITHMS BOXES ------------------
    std::vector<sfg::CheckButton::Ptr> activeAlgCheckButtons;
    for (auto &pathfinder : m_pathfinderMgr.GetPathfinders())
        activeAlgCheckButtons.push_back(sfg::CheckButton::Create(pathfinder->GetName()));

    auto activeAlgCheckButtonsBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 3.0f);
    for (auto &checkButton : activeAlgCheckButtons)
    {
        checkButton->GetSignal(sfg::ToggleButton::OnToggle).Connect([checkButton, this] {
            checkButton->IsActive() ? m_pathfinderMgr.Activate(checkButton->GetLabel()) : m_pathfinderMgr.Deactivate(checkButton->GetLabel());
        });
        checkButton->SetActive(true);
        activeAlgCheckButtonsBox->Pack(checkButton);
    }

    // -------------- DRAW SETTINGS CHECK BOXES -----------------
    auto drawWorkerCheckButton = sfg::CheckButton::Create("Worker");
    auto drawConnectionsCheckButton = sfg::CheckButton::Create("Connections");
    auto drawNeighborsCheckButton = sfg::CheckButton::Create("Neighbors");

    auto drawSettingsCheckButtonsBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 3.0f);
    drawSettingsCheckButtonsBox->Pack(drawWorkerCheckButton);
    drawSettingsCheckButtonsBox->Pack(drawConnectionsCheckButton);
    drawSettingsCheckButtonsBox->Pack(drawNeighborsCheckButton);

    drawWorkerCheckButton->GetSignal(sfg::ToggleButton::OnToggle).Connect([this, drawWorkerCheckButton] { m_pathfinderMgr.SetDrawWorker(drawWorkerCheckButton->IsActive()); });
    drawConnectionsCheckButton->GetSignal(sfg::ToggleButton::OnToggle).Connect([this, drawConnectionsCheckButton] { m_pathfinderMgr.SetDrawViaConnections(drawConnectionsCheckButton->IsActive()); });
    drawNeighborsCheckButton->GetSignal(sfg::ToggleButton::OnToggle).Connect([this, drawNeighborsCheckButton] { m_pathfinderMgr.SetDrawNeighbors(drawNeighborsCheckButton->IsActive()); });

    drawWorkerCheckButton->SetActive(true);
    drawConnectionsCheckButton->SetActive(true);
    drawNeighborsCheckButton->SetActive(false);

    // -------------- EDIT MODE RADIO BUTTONS -------------------
    auto editstateNone = sfg::RadioButton::Create("None");
    auto editstateAddObs = sfg::RadioButton::Create("Add obstacle", editstateNone->GetGroup());
    auto editstateRemObs = sfg::RadioButton::Create("Remove obstacle", editstateNone->GetGroup());
    auto editstateSetStart = sfg::RadioButton::Create("Set start", editstateNone->GetGroup());
    auto editstateSetGoal = sfg::RadioButton::Create("Set goal", editstateNone->GetGroup());

    editstateNone->GetSignal(sfg::ToggleButton::OnToggle).Connect([this] { m_pathfinderMgr.SetEditState(PathfinderMgr::EditState::None); });
    editstateAddObs->GetSignal(sfg::ToggleButton::OnToggle).Connect([this] { m_pathfinderMgr.SetEditState(PathfinderMgr::EditState::AddObstacles); });
    editstateRemObs->GetSignal(sfg::ToggleButton::OnToggle).Connect([this] { m_pathfinderMgr.SetEditState(PathfinderMgr::EditState::RemObstacles); });
    editstateSetStart->GetSignal(sfg::ToggleButton::OnToggle).Connect([this] { m_pathfinderMgr.SetEditState(PathfinderMgr::EditState::SetStart); });
    editstateSetGoal->GetSignal(sfg::ToggleButton::OnToggle).Connect([this] { m_pathfinderMgr.SetEditState(PathfinderMgr::EditState::SetGoal); });

    editstateNone->SetActive(true);

    auto editstateBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 3.0f);
    editstateBox->Pack(editstateNone);
    editstateBox->Pack(editstateAddObs);
    editstateBox->Pack(editstateRemObs);
    editstateBox->Pack(editstateSetStart);
    editstateBox->Pack(editstateSetGoal);

    // -------------- ALL BUTTONS ------------------
    auto startButton = sfg::Button::Create("Start");
    auto restartButton = sfg::Button::Create("Restart");
    auto resetButton = sfg::Button::Create("Reset");
    auto pauseButton = sfg::Button::Create("Pause");
    auto resumeButton = sfg::Button::Create("Resume");

    startButton->GetSignal(sfg::Widget::OnLeftClick).Connect([this] { m_pathfinderMgr.Start(); });
    restartButton->GetSignal(sfg::Widget::OnLeftClick).Connect([this] { m_pathfinderMgr.Restart(); });
    resetButton->GetSignal(sfg::Widget::OnLeftClick).Connect([this] { m_pathfinderMgr.Reset(); });
    pauseButton->GetSignal(sfg::Widget::OnLeftClick).Connect([this] { m_pathfinderMgr.Pause(); });
    resumeButton->GetSignal(sfg::Widget::OnLeftClick).Connect([this] { m_pathfinderMgr.Resume(); });

    auto buttonsBoxRow0 = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 3.0f);
    auto buttonsBoxRow1 = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 3.0f);

    buttonsBoxRow0->Pack(startButton);
    buttonsBoxRow0->Pack(restartButton);
    buttonsBoxRow0->Pack(resetButton);
    buttonsBoxRow1->Pack(pauseButton);
    buttonsBoxRow1->Pack(resumeButton);

    auto allButtonBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5.0f);
    allButtonBox->Pack(buttonsBoxRow0, false);
    allButtonBox->Pack(buttonsBoxRow1, false);

    // ------------- VisType, Voronoi/Squares.. --------------
    auto visStyleComboBox = sfg::ComboBox::Create();
    visStyleComboBox->AppendItem("Squares");
    visStyleComboBox->AppendItem("Voronoi");
    visStyleComboBox->SelectItem(0);

    visStyleComboBox->GetSignal(sfg::ComboBox::OnSelect).Connect([visStyleComboBox, this] {
        const auto selectedItem = visStyleComboBox->GetSelectedItem();
        switch (selectedItem)
        {
        case 0:
            m_pathfinderMgr.SetVisType(TraverseGrid::Type::Square);
            break;
        case 1:
            m_pathfinderMgr.SetVisType(TraverseGrid::Type::Voronoi);
            break;
        default:
            break;
        }
    });

    // --------------- SUB BOXES ----------------------
    auto boxGrid = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 4.0f);
    boxGrid->Pack(labelGrid);
    boxGrid->Pack(visStyleComboBox);

    auto boxAlgorithm = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 4.0f);
    boxAlgorithm->Pack(labelAlgorithms);
    boxAlgorithm->Pack(activeAlgCheckButtonsBox);

    auto boxEditState = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 4.0f);
    boxEditState->Pack(labelTools);
    boxEditState->Pack(editstateBox);

    auto boxSettings = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 4.0f);
    boxSettings->Pack(labelSettings);
    boxSettings->Pack(scaleBox);
    boxSettings->Pack(drawSettingsCheckButtonsBox);

    // -------------- ADD TO MAIN BOX ------------------
    auto mainBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 15.0f);
    mainBox->Pack(allButtonBox, false);
    mainBox->Pack(boxGrid, false);
    mainBox->Pack(boxAlgorithm, false);
    mainBox->Pack(boxEditState, false);
    mainBox->Pack(boxSettings, false);
    mainBox->Pack(boxSettings, false);

    // -------------- ADD TO MAIN WINDOW ------------------
    auto window = sfg::Window::Create(sfg::Window::Style::BACKGROUND);
    window->SetPosition(sf::Vector2f(Window::GetWidth() - 200.0f, 0.0f));
    window->SetRequisition(sf::Vector2f(200.0f, Window::GetHeight()));
    window->Add(mainBox);

    GuiMgr::Add(window);
}

void ClientMainScreen::OnExit()
{
}

void ClientMainScreen::Update()
{
    m_pathfinderMgr.Update();
}

void ClientMainScreen::Draw()
{
    m_pathfinderMgr.DrawGrid();
    m_pathfinderMgr.DrawPathfinders();
}

int ClientMainScreen::GetNextScreenIndex() const
{
    return SCREENINDEX_NO_SCREEN;
}

int ClientMainScreen::GetPreviousScreenIndex() const
{
    return SCREENINDEX_NO_SCREEN;
}