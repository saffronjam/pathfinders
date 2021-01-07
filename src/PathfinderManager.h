#pragma once

#include <Saffron.h>

#include "Pathfinder.h"

namespace Se
{
class PathfinderManager
{
public:
	enum class EditState
	{
		None,
		AddObstacles,
		RemObstacles,
		SetStart,
		SetGoal,
		AddSubGoal,
		RemSubGoal
	};

public:
	explicit PathfinderManager();

	void OnUpdate(Scene &scene);
	void OnRenderGrid(Scene &scene);
	void OnRenderPathfinders(Scene &scene);
	void OnGuiRender();
	void OnRenderTargetResize(const sf::Vector2f &size);

	void Start();
	void Pause();
	void Resume();
	void Restart();
	void Reset();

	const auto &GetPathfinders() const { return _pathfinders; }
	EditState GetEditState() const { return _editState; }
	const sf::Time &GetTimer() const { return _timer; }
	Pathfinder::State GetState() const { return GetActivePathfinder()->GetState(); }

	void SetSleepDelay(sf::Time delay);
	void SetEditState(EditState editState) { _editState = editState; }

	void SetActivePathfinder(const String &name);
	const Unique<Pathfinder> &GetActivePathfinder() const { return *_activePathFinder; }

	void SetActiveTraverseGrid(const String &name);
	const Shared<TraverseGrid> &GetActiveTraverseGrid() const { return *_activeTraverseGrid; }

	void ClearTimerResults() { _oldResults.clear(); }

private:
	void ResetTimer() { _timer = sf::Time::Zero; }
	String FormatTimerValue();
	void PushTimerToResultStack();

	template<class T>
	auto SetActiveHelper(ArrayList<T> &list, const String &name);

private:
	EditState _editState;

	ArrayList<Shared<TraverseGrid>> _traverseGrids;
	ArrayList<Shared<TraverseGrid>>::iterator _activeTraverseGrid;

	ArrayList<Unique<Pathfinder>> _pathfinders;
	ArrayList<Unique<Pathfinder>>::iterator _activePathFinder;;

	bool _drawWorker;
	bool _drawViaConnections;
	bool _drawNeighbors;

	sf::Time _timer;
	std::deque<String> _oldResults;
};

template <class T>
auto PathfinderManager::SetActiveHelper(ArrayList<T> &list, const String &name)
{
	const auto candidate = std::find_if(list.begin(), list.end(),
										[&name](const auto &current)
										{
											return current->GetName() == name;
										});

	SE_CORE_ASSERT(candidate != list.end(), "Invalid name");

	auto iter = list.begin();
	std::advance(iter, std::distance(list.begin(), candidate));

	return iter;
}
}
