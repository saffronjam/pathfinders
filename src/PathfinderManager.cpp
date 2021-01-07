#include "PathfinderManager.h"

#include "Grids/SquareGrid.h"
#include "Grids/VoronoiGrid.h"

#include "Pathfinders/AStar.h"
#include "Pathfinders/Dijkstra.h"
#include "Pathfinders/BestFirstSearch.h"
#include "Pathfinders/Beam32.h"
#include "Pathfinders/Beam512.h"

namespace Se
{
PathfinderManager::PathfinderManager() :
	_editState(EditState::None),
	_activeTraverseGrid(_traverseGrids.end()),
	_activePathFinder(_pathfinders.end()),
	_drawWorker(true),
	_drawViaConnections(true),
	_drawNeighbors(false)
{
	_traverseGrids.push_back(std::make_shared<SquareGrid>());
	_traverseGrids.push_back(std::make_shared<VoronoiGrid>());
	SetActiveTraverseGrid("Square");

	_pathfinders.push_back(std::make_unique<AStar>());
	_pathfinders.push_back(std::make_unique<Dijkstra>());
	_pathfinders.push_back(std::make_unique<BestFirstSearch>());
	_pathfinders.push_back(std::make_unique<Beam32>());
	_pathfinders.push_back(std::make_unique<Beam512>());
	SetActivePathfinder("A*");

	const auto activeGrid = GetActiveTraverseGrid();
	for ( auto &pathfinder : _pathfinders )
	{
		pathfinder->SetTraverseGrid(activeGrid);
		pathfinder->AssignNodes(activeGrid->GetNodes());
	}
}

void PathfinderManager::OnUpdate(Scene &scene)
{
	if ( Mouse::IsDown(sf::Mouse::Button::Left) && scene.GetViewportPane().IsHovered() )
	{
		const auto &activeGrid = GetActiveTraverseGrid();
		const int nodeUID = activeGrid->GetNodeUID(scene.GetCamera().ScreenToWorld(Mouse::GetPosition()));
		switch ( _editState )
		{
		case EditState::AddObstacles:
		{
			activeGrid->AddObstacle(nodeUID);
			break;
		}
		case EditState::RemObstacles:
		{
			activeGrid->RemoveObstacle(nodeUID);
			break;
		}
		case EditState::SetStart:
		{
			activeGrid->SetStart(nodeUID);
			break;
		}
		case EditState::SetGoal:
		{
			activeGrid->SetGoal(nodeUID);
			break;
		}
		case EditState::AddSubGoal:
		{
			activeGrid->AddSubGoal(nodeUID);
			break;
		}
		case EditState::RemSubGoal:
		{
			activeGrid->RemoveSubGoal(nodeUID);
			break;
		}
		default:
			break;
		}
	}

	//if ( _activePathFinder->GetState() == Pathfinder::State::Finding )
	//	_timer += Clock::Delta();
	//String timerAll(FormatTimerValue());
	//for ( auto &result : _oldResults )
	//	timerAll += ' ' + result;
	//timerLabel->SetText(timerAll);
	//timerLabel->SetLineWrap(true);
}

void PathfinderManager::OnRenderGrid(Scene &scene)
{
	GetActiveTraverseGrid()->OnRender(scene);
}

void PathfinderManager::OnRenderPathfinders(Scene &scene)
{
	const auto &activeFinder = GetActivePathfinder();
	if ( _drawNeighbors )
	{
		activeFinder->OnRenderNeighbors(scene);
	}
	if ( _drawViaConnections )
	{
		activeFinder->OnRenderViaConnections(scene);
	}
	if ( _drawWorker )
	{
		if ( !activeFinder->IsDone() )
		{
			activeFinder->OnRenderAnticipation(scene);
		}
		else
		{
			activeFinder->OnRenderResult(scene);
		}
	}
}

void PathfinderManager::OnGuiRender()
{
}

void PathfinderManager::OnRenderTargetResize(const sf::Vector2f &size)
{
	for ( auto &grid : _traverseGrids )
	{
		grid->OnRenderTargetResize(size);
	}
}

void PathfinderManager::Start()
{
	const auto &activeFinder = GetActivePathfinder();
	const auto &activeGrid = GetActiveTraverseGrid();
	if ( activeFinder->GetState() == Pathfinder::State::WaitingForStart )
	{
		PushTimerToResultStack();
		ResetTimer();
	}
	activeFinder->Start(activeGrid->GetStartUID(), activeGrid->GetGoalUID(), activeGrid->GetSubGoalUIDs());
}

void PathfinderManager::Pause()
{
	GetActivePathfinder()->Pause();
}

void PathfinderManager::Resume()
{
	GetActivePathfinder()->Resume();
}

void PathfinderManager::Restart()
{
	GetActivePathfinder()->Restart();
}

void PathfinderManager::Reset()
{
	for ( auto &pathfinder : _pathfinders )
	{
		pathfinder->Reset();
	}
	const auto &activeGrid = GetActiveTraverseGrid();
	activeGrid->ClearObstacles();
	activeGrid->ClearSubGoals();
	activeGrid->ResetStartGoal();
}

void PathfinderManager::SetSleepDelay(sf::Time delay)
{
	for ( auto &pathfinder : _pathfinders )
	{
		pathfinder->SetSleepDelay(delay);
	}
}

void PathfinderManager::SetActivePathfinder(const String &name)
{
	_activePathFinder = SetActiveHelper(_pathfinders, name);
}

void PathfinderManager::SetActiveTraverseGrid(const String &name)
{
	_activeTraverseGrid = SetActiveHelper(_traverseGrids, name);
}

String PathfinderManager::FormatTimerValue()
{
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(5) << "\t\n"
		<< _timer.asSeconds() << "s (" << GetActivePathfinder()->GetName() << ")";
	return oss.str();
}

void PathfinderManager::PushTimerToResultStack()
{
	if ( _timer != sf::Time::Zero )
	{
		if ( _oldResults.size() > 5 )
			_oldResults.pop_back();
		_oldResults.push_front(FormatTimerValue());
	}
}
}