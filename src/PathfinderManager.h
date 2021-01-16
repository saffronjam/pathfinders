#pragma once

#include <Saffron.h>

#include "Pathfinder.h"

namespace Se
{
class PathfinderManager : public Signaller
{
public:
	enum class EditState
	{
		None,
		Obstacles,
		SubGoal,
		Weights,
		Start,
		Goal,
		Count
	};

public:
	explicit PathfinderManager();

	void OnUpdate(Scene &scene);

	void OnRender(Scene &scene);
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

	void SetSleepDelay(sf::Time delay);
	void SetEditState(EditState editState) { _editState = editState; }
	void SetWeight(int uidFirst, int uidSecond, float weight);

	void SetActiveTraverseGrid(const String &name);
	Shared<TraverseGrid> &GetActiveTraverseGrid() { return *_activeTraverseGrid; }
	const Shared<TraverseGrid> &GetActiveTraverseGrid() const { return *_activeTraverseGrid; }

	void ClearTimerResults() { _oldResults.clear(); }

private:
	template<class T>
	auto SetActiveHelper(ArrayList<T> &list, const String &name);

	ArrayList<ArrayList<Unique<Pathfinder>>::iterator> GetActivePathfinders();

	void CollectWorker();

private:
	EditState _editState;

	Thread _worker;
	Atomic<bool> _finishedWorking = false;
	Atomic<bool> _didOnFinishWorkingUpdate = false;

	ArrayList<Shared<TraverseGrid>> _traverseGrids;
	ArrayList<Shared<TraverseGrid>>::iterator _activeTraverseGrid;

	ArrayList<Unique<Pathfinder>> _pathfinders;

	bool _drawWorker = true;
	bool _drawViaConnections = true;
	bool _drawNeighbors = false;

	sf::Time _timer;
	Deque<String> _oldResults;

	Pair<int, int> _weightEditPair = { 0,0 };

	// Cache
	sf::Vector2f _renderTargetSize = { 0.0f, 0.0f };

	// Gui cache
	ArrayList<const char *> _traverseGridNames;
	int _activeTraverseGridIndex = 0;
	float _sleepDelayMicroseconds = 10000;
	ArrayList<const char *> _editStateNames;
	int _editStateIndex = static_cast<int>(EditState::None);

	bool _drawWeights = false;
	bool _drawFadedWeights = false;
	float _weight = 1.0f;
	bool _weightBrushEnabled = false;
	int _weightBrushSize = 10;

	bool _obstacleBrushEnabled = false;
	int _obstacleBrushSize = 10;

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
