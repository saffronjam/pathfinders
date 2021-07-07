#pragma once

#include <Saffron.h>

#include "Pathfinder.h"

namespace Se
{
enum class PathfinderManagerEditState
{
	None,
	Obstacles,
	SubGoal,
	Weights,
	Start,
	Goal,
	Count
};

class PathfinderManager
{
public:
	PathfinderManager();

	void OnUpdate(Scene& scene);

	void OnRender(Scene& scene);
	void OnRenderPathfinders(Scene& scene);
	void OnGuiRender();
	void OnRenderTargetResize(const sf::Vector2f& size);
	void OnExit();

	void Start();
	void Pause();
	void Resume();
	void Restart();
	void Reset();

	auto Pathfinders() const -> const List<Unique<Pathfinder>>&;

	auto EditState() const -> PathfinderManagerEditState;
	auto RunningDuration() const -> const sf::Time&;

	void SetSleepDelay(sf::Time delay);
	void SetEditState(PathfinderManagerEditState editState);
	void SetWeight(int uidFirst, int uidSecond, float weight);

	void SetActiveTraverseGrid(const String& name);
	auto ActiveTraverseGrid() -> Shared<TraverseGrid>&;
	auto ActiveTraverseGrid() const -> const Shared<TraverseGrid>&;

	void ClearTimerResults();

private:
	template <class T>
	auto SetActiveHelper(List<T>& list, const String& name);

	auto ActivePathfinders() -> List<List<Unique<Pathfinder>>::iterator>;

private:
	PathfinderManagerEditState _editState;

	ThreadPool _threadPool;
	Mutex _workerMutex;

	struct ScopedBool
	{
		explicit ScopedBool(Atomic<bool>& value) :
			_value(value)
		{
			value = true;
		}

		~ScopedBool()
		{
			_value = false;
		}

	private:
		Atomic<bool>& _value;
	};

	Atomic<bool> _didOnFinishWorkingUpdate = false;
	Atomic<bool> _working = false;

	List<Shared<TraverseGrid>> _traverseGrids;
	List<Shared<TraverseGrid>>::iterator _activeTraverseGrid;

	List<Unique<Pathfinder>> _pathfinders;

	bool _drawWorker = true;
	bool _drawViaConnections = true;

	sf::Time _runningDuration;
	Deque<String> _oldResults;

	Pair<int, int> _editPair = {0, 0};

	// Cache
	sf::Vector2f _renderTargetSize{0.0f, 0.0f};
	sf::Vector2f _desiredRenderTargetSize{0.0f, 0.0f};

	// Gui cache
	List<const char*> _traverseGridNames;
	int _activeTraverseGridIndex = 0;
	float _sleepDelayMicroseconds = 10000;
	List<const char*> _editStateNames;
	int _editStateIndex = static_cast<int>(PathfinderManagerEditState::None);

	bool _drawWeights = false;
	bool _drawFadedWeights = false;
	float _weight = 1.0f;
	bool _weightBrushEnabled = false;
	int _weightBrushSize = 10;

	bool _obstacleBrushEnabled = false;
	int _obstacleBrushSize = 10;

	int _mazeNewPaths = 0;
};

template <class T>
auto PathfinderManager::SetActiveHelper(List<T>& list, const String& name)
{
	const auto candidate = std::find_if(list.begin(), list.end(), [&name](const auto& current)
	{
		return current->Name() == name;
	});

	Debug::Assert(candidate != list.end(), "Invalid name");

	auto iter = list.begin();
	std::advance(iter, std::distance(list.begin(), candidate));

	return iter;
}
}
