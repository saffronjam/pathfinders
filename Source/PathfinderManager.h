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

	auto Pathfinders() const -> const std::vector<std::unique_ptr<Pathfinder>>&;

	auto EditState() const -> PathfinderManagerEditState;
	auto RunningDuration() const -> const sf::Time&;

	void SetSleepDelay(sf::Time delay);
	void SetEditState(PathfinderManagerEditState editState);
	void SetWeight(int uidFirst, int uidSecond, float weight);

	void SetActiveTraverseGrid(const std::string& name);
	auto ActiveTraverseGrid() -> std::shared_ptr<TraverseGrid>&;
	auto ActiveTraverseGrid() const -> const std::shared_ptr<TraverseGrid>&;

	void ClearTimerResults();

private:
	template <class T>
	auto SetActiveHelper(std::vector<T>& list, const std::string& name);

	auto ActivePathfinders() -> std::vector<std::vector<std::unique_ptr<Pathfinder>>::iterator>;

private:
	PathfinderManagerEditState _editState;

	ThreadPool _threadPool;
	std::mutex _workerMutex;

	struct ScopedBool
	{
		explicit ScopedBool(std::atomic<bool>& value) :
			_value(value)
		{
			value = true;
		}

		~ScopedBool()
		{
			_value = false;
		}

	private:
		std::atomic<bool>& _value;
	};

	std::atomic<bool> _didOnFinishWorkingUpdate = false;
	std::atomic<bool> _working = false;

	std::vector<std::shared_ptr<TraverseGrid>> _traverseGrids;
	std::vector<std::shared_ptr<TraverseGrid>>::iterator _activeTraverseGrid;

	std::vector<std::unique_ptr<Pathfinder>> _pathfinders;

	bool _drawWorker = true;
	bool _drawViaConnections = true;

	sf::Time _runningDuration;
	std::deque<std::string> _oldResults;

	std::pair<int, int> _editPair = {0, 0};

	// Cache
	sf::Vector2f _renderTargetSize{0.0f, 0.0f};
	sf::Vector2f _desiredRenderTargetSize{0.0f, 0.0f};

	// Gui cache
	std::vector<const char*> _traverseGridNames;
	int _activeTraverseGridIndex = 0;
	float _sleepDelayMicroseconds = 10000;
	std::vector<const char*> _editStateNames;
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
auto PathfinderManager::SetActiveHelper(std::vector<T>& list, const std::string& name)
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
