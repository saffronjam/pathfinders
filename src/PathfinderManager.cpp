#include "PathfinderManager.h"

#include "Grids/SquareGrid.h"
#include "Grids/VoronoiGrid.h"

#include "Pathfinders/AStar.h"
#include "Pathfinders/Dijkstra.h"
#include "Pathfinders/BestFirstSearch.h"
#include "Pathfinders/Beam.h"
#include "Pathfinders/BFS.h"
#include "Pathfinders/DFS.h"

namespace Se
{
PathfinderManager::PathfinderManager() :
	_editState(PathfinderManagerEditState::None)
{
	_pathfinders.push_back(CreateUnique<AStar>());
	_pathfinders.push_back(CreateUnique<Dijkstra>());
	_pathfinders.push_back(CreateUnique<BestFirstSearch>());
	_pathfinders.push_back(CreateUnique<Beam<32>>());
	_pathfinders.push_back(CreateUnique<Beam<512>>());
	_pathfinders.push_back(CreateUnique<BFS>());
	_pathfinders.push_back(CreateUnique<DFS>());

	_traverseGrids.push_back(CreateShared<SquareGrid>());
	_traverseGrids.push_back(CreateShared<VoronoiGrid>());
	_activeTraverseGrid = _traverseGrids.end();

	SetActiveTraverseGrid("Square");


	const auto activeGrid = ActiveTraverseGrid();
	for (auto& pathfinder : _pathfinders)
	{
		pathfinder->SetTraverseGrid(activeGrid);
		pathfinder->AssignNodes(activeGrid->Nodes());
	}

	for (const auto& traverseGrid : _traverseGrids)
	{
		_traverseGridNames.push_back(traverseGrid->Name().c_str());
	}

	_editStateNames.resize(static_cast<int>(PathfinderManagerEditState::Count));

	_editStateNames[static_cast<int>(PathfinderManagerEditState::None)] = "None";
	_editStateNames[static_cast<int>(PathfinderManagerEditState::Obstacles)] = "Obstacles";
	_editStateNames[static_cast<int>(PathfinderManagerEditState::SubGoal)] = "Subgoal";
	_editStateNames[static_cast<int>(PathfinderManagerEditState::Weights)] = "Weight";
	_editStateNames[static_cast<int>(PathfinderManagerEditState::Start)] = "Start";
	_editStateNames[static_cast<int>(PathfinderManagerEditState::Goal)] = "Goal";
}

PathfinderManager::~PathfinderManager()
{
	CollectWorker();
}

void PathfinderManager::OnUpdate(Scene& scene)
{
	const bool shiftDown = Keyboard::IsDown(sf::Keyboard::Key::LShift) || Keyboard::IsDown(sf::Keyboard::Key::RShift);
	const auto& activeGrid = ActiveTraverseGrid();

	if (_finishedWorking && !_didOnFinishWorkingUpdate)
	{
		CollectWorker();

		for (auto& pathfinder : _pathfinders)
		{
			pathfinder->AssignNodes(activeGrid->Nodes());
			pathfinder->SetTraverseGrid(activeGrid);
		}

		_didOnFinishWorkingUpdate = true;
	}


	if (_drawWeights)
	{
		activeGrid->SetWeightColorAlpha(255);
		activeGrid->AddDrawFlags(TraverseGridDrawFlag_Weights);
	}
	else if (_drawFadedWeights)
	{
		activeGrid->SetWeightColorAlpha(60);
		activeGrid->AddDrawFlags(TraverseGridDrawFlag_Weights);
	}
	else
	{
		activeGrid->RemoveDrawFlags(TraverseGridDrawFlag_Weights);
	}


	if (_finishedWorking)
	{
		// Reset before check hovered
		if (activeGrid->IsEdgeClear(_editPair.first, _editPair.second))
		{
			activeGrid->ClearNodeEdgeColor(_editPair.first, _editPair.second);
		}

		if (scene.ViewportPane().Hovered())
		{
			const auto mouseInViewportPanePosition = scene.Camera().ScreenToWorld(scene.ViewportPane().MousePosition());
			const int nodeUID = activeGrid->NodeUidByPosition(mouseInViewportPanePosition);


			_editPair.first = nodeUID;
			_editPair.second = activeGrid->ClosestNeighborUID(nodeUID, mouseInViewportPanePosition);

			if (activeGrid->IsEdgeClear(_editPair.first, _editPair.second))
			{
				if (_editState == PathfinderManagerEditState::Weights && !_weightBrushEnabled)
				{
					const auto color = TraverseGrid::WeightColor(_weight);
					activeGrid->SetNodeEdgeColor(_editPair.first, _editPair.second, color);
				}
				if (_editState == PathfinderManagerEditState::Obstacles && !_obstacleBrushEnabled)
				{
					const auto color = activeGrid->GridColor();
					activeGrid->SetNodeEdgeColor(_editPair.first, _editPair.second, color);
				}
			}

			if (Mouse::IsDown(sf::Mouse::Button::Left))
			{
				switch (_editState)
				{
				case PathfinderManagerEditState::Obstacles:
				{
					if (_obstacleBrushEnabled)
					{
						for (const auto& [uid, node] : activeGrid->Nodes())
						{
							if (VecUtils::LengthSq(node.Position() - mouseInViewportPanePosition) < std::pow(
								static_cast<float>(_obstacleBrushSize), 2.0f))
							{
								for (const auto& neighborUID : node.Neighbors())
								{
									shiftDown
										? activeGrid->RemoveObstacle(uid, neighborUID)
										: activeGrid->AddObstacle(uid, neighborUID);
								}
							}
						}
					}
					else
					{
						shiftDown
							? activeGrid->RemoveObstacle(_editPair.first, _editPair.second)
							: activeGrid->AddObstacle(_editPair.first, _editPair.second);
					}
					break;
				}
				case PathfinderManagerEditState::Start:
				{
					activeGrid->SetStart(nodeUID);
					break;
				}
				case PathfinderManagerEditState::Goal:
				{
					activeGrid->SetGoal(nodeUID);
					break;
				}
				case PathfinderManagerEditState::SubGoal:
				{
					shiftDown ? activeGrid->RemoveSubGoal(nodeUID) : activeGrid->AddSubGoal(nodeUID);
					break;
				}
				case PathfinderManagerEditState::Weights:
				{
					const float weight = shiftDown ? 1.0f : _weight;
					if (_weightBrushEnabled)
					{
						for (const auto& [uid, node] : activeGrid->Nodes())
						{
							if (VecUtils::LengthSq(node.Position() - mouseInViewportPanePosition) < std::pow(
								static_cast<float>(_weightBrushSize), 2.0f))
							{
								for (const auto& neighborUID : node.Neighbors())
								{
									SetWeight(uid, neighborUID, weight);
								}
							}
						}
					}
					else
					{
						SetWeight(_editPair.first, _editPair.second, weight);
					}
					break;
				}
				default: break;
				}
			}
		}


		ActiveTraverseGrid()->OnUpdate();

		for (auto& pathfinder : _pathfinders)
		{
			pathfinder->OnUpdate();
		}
		
		if (VecUtils::LengthSq(_renderTargetSize - _desiredRenderTargetSize) > 0.1f)
		{
			Reset();

			CollectWorker();

			_finishedWorking = false;
			_worker = Thread([this]
				{
					while (!_allowedToWork)
					{
					}
					ActiveTraverseGrid()->OnRenderTargetResize(_desiredRenderTargetSize);
					_allowedToWork = false;
					_didOnFinishWorkingUpdate = false;
					_finishedWorking = true;
				});

			_renderTargetSize = _desiredRenderTargetSize;
		}
	}
}

void PathfinderManager::OnRender(Scene& scene)
{
	_allowedToWork = !_finishedWorking;

	ActiveTraverseGrid()->OnRender(scene);

	if (!_allowedToWork)
	{
		OnRenderPathfinders(scene);

		if (_editState == PathfinderManagerEditState::Obstacles && _obstacleBrushEnabled)
		{
			const sf::Color color(255, 0, 0, 50);
			const auto position = scene.Camera().ScreenToWorld(scene.ViewportPane().MousePosition());
			scene.Submit(position, color, static_cast<float>(_obstacleBrushSize));
		}

		if (_editState == PathfinderManagerEditState::Weights && _weightBrushEnabled)
		{
			const sf::Color color(0, 255, 0, 50);
			const auto position = scene.Camera().ScreenToWorld(scene.ViewportPane().MousePosition());
			scene.Submit(position, color, static_cast<float>(_weightBrushSize));
		}
	}

	_allowedToWork = true;
}

void PathfinderManager::OnRenderPathfinders(Scene& scene)
{
	auto activePathfinder = ActivePathfinders();

	if (_drawViaConnections)
	{
		for (auto& pathfinder : activePathfinder)
		{
			(*pathfinder)->OnRenderViaConnections(scene);
		}
	}
	if (_drawWorker)
	{
		for (auto& pathfinder : activePathfinder)
		{
			(*pathfinder)->OnRenderBody(scene);
		}
	}
}

void PathfinderManager::OnGuiRender()
{
	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, !_finishedWorking);

	auto activeGrid = ActiveTraverseGrid();

	Gui::BeginPropertyGrid("Grid");
	ImGui::Text("Traverse Grid");
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);
	if (ImGui::Combo("##Traverse Grid", &_activeTraverseGridIndex, _traverseGridNames.data(),
	                 _traverseGridNames.size()))
	{
		SetActiveTraverseGrid(_traverseGridNames.at(_activeTraverseGridIndex));
	}
	ImGui::NextColumn();

	ImGui::Columns(3, "StartRestartReset");

	if (ImGui::Button("Start", {ImGui::GetContentRegionAvailWidth(), 0}))
	{
		Start();
	}
	ImGui::NextColumn();
	if (ImGui::Button("Restart", {ImGui::GetContentRegionAvailWidth(), 0}))
	{
		Restart();
	}
	ImGui::NextColumn();
	if (ImGui::Button("Reset", {ImGui::GetContentRegionAvailWidth(), 0}))
	{
		Reset();
	}

	ImGui::Columns(2, "PauseResume");

	if (ImGui::Button("Pause", {ImGui::GetContentRegionAvailWidth(), 0}))
	{
		Pause();
	}
	ImGui::NextColumn();
	if (ImGui::Button("Resume", {ImGui::GetContentRegionAvailWidth(), 0}))
	{
		Resume();
	}
	ImGui::NextColumn();

	Gui::EndPropertyGrid();

	ImGui::Separator();

	ImGui::Columns(1, "SquareGenerateMaze");
	if (ImGui::Button("Generate Maze", {ImGui::GetContentRegionAvailWidth(), 0}))
	{
		CollectWorker();
		_finishedWorking = false;
		_worker = Thread([this]
		{
			while (!_allowedToWork)
			{
			}
			Reset();
			ActiveTraverseGrid()->GenerateMaze();
			_allowedToWork = false;
			_didOnFinishWorkingUpdate = false;
			_finishedWorking = true;
		});
	}
	Gui::BeginPropertyGrid("MazeGeneration");
	const float min = 0, max = 1000;
	if (Gui::Property("New paths", _mazeNewPaths, min, max, 1, GuiPropertyFlag_Slider))
	{
		activeGrid->SetNoWallsToSmash(_mazeNewPaths);
	}
	Gui::EndPropertyGrid();

	ImGui::Separator();

	Gui::BeginPropertyGrid("Time");
	if (Gui::Property("Sleep delay", _sleepDelayMicroseconds, 0.0f, 1000000.0f, 1.0f,
	                  GuiPropertyFlag_Slider | GuiPropertyFlag_Logarithmic))
	{
		SetSleepDelay(sf::microseconds(_sleepDelayMicroseconds));
	}


	const auto drawFlags = activeGrid->DrawFlags();
	bool drawGrid = drawFlags & TraverseGridDrawFlag_Grid;
	bool drawObjects = drawFlags & TraverseGridDrawFlag_Objects;

	if (Gui::Property("Grid", drawGrid))
	{
		activeGrid->SetDrawFlags(drawGrid
			                         ? drawFlags | TraverseGridDrawFlag_Grid
			                         : drawFlags - TraverseGridDrawFlag_Grid);
	}
	if (Gui::Property("Objects", drawObjects))
	{
		activeGrid->SetDrawFlags(drawObjects
			                         ? drawFlags | TraverseGridDrawFlag_Objects
			                         : drawFlags - TraverseGridDrawFlag_Objects);
	}
	Gui::Property("Weights", _drawFadedWeights);

	Gui::EndPropertyGrid();

	ImGui::Separator();

	Gui::BeginPropertyGrid("Edit");

	String editText = "Edit";

	switch (_editState)
	{
	case PathfinderManagerEditState::Obstacles:
	case PathfinderManagerEditState::SubGoal: editText += " (Hold shift to remove)";
		break;
	case PathfinderManagerEditState::Weights: editText += " (Hold shift to apply reset)";
		break;
	default: break;
	}

	ImGui::Text(editText.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);
	if (ImGui::Combo("##EditState", &_editStateIndex, _editStateNames.data(), _editStateNames.size()))
	{
		_drawWeights = static_cast<PathfinderManagerEditState>(_editStateIndex) == PathfinderManagerEditState::Weights;
		SetEditState(static_cast<PathfinderManagerEditState>(_editStateIndex));
	}

	ImGui::NextColumn();

	switch (_editState)
	{
	case PathfinderManagerEditState::Obstacles: Gui::Property("Enable Brush", _obstacleBrushEnabled);
		if (_obstacleBrushEnabled)
		{
			Gui::Property("Brush Size", _obstacleBrushSize, 1, 100, 1, GuiPropertyFlag_Slider);
		}
		break;
	case PathfinderManagerEditState::Weights: Gui::Property("Weight", _weight, 0, TraverseGrid::MaxWeight, 1,
	                                                        GuiPropertyFlag_Slider);
		Gui::Property("Enable Brush", _weightBrushEnabled);
		if (_weightBrushEnabled)
		{
			Gui::Property("Brush Size", _weightBrushSize, 1, 100, 1, GuiPropertyFlag_Slider);
		}
		break;
	default: break;
	}

	Gui::EndPropertyGrid();

	ImGui::Separator();


	ImGui::Columns(4, "Pathfinders");

	for (auto& pathfinder : _pathfinders)
	{
		const String& name = pathfinder->Name();
		ImGui::Text(name.c_str());

		ImGui::NextColumn();

		String id = "##" + name;
		bool active = pathfinder->Active();
		if (ImGui::Checkbox(id.c_str(), &active))
		{
			active ? pathfinder->Activate() : pathfinder->Deactivate();
		}

		ImGui::SameLine();

		id += "Color";
		const sf::Color body = pathfinder->BodyColor();
		float color[4] = {
			static_cast<float>(body.r) / 255.0f, static_cast<float>(body.g) / 255.0f,
			static_cast<float>(body.b) / 255.0f, static_cast<float>(body.a) / 255.0f
		};

		if (ImGui::ColorEdit4(id.c_str(), color, ImGuiColorEditFlags_NoInputs))
		{
			const sf::Color newColor(color[0] * 255, color[1] * 255, color[2] * 255, color[3] * 255);
			pathfinder->SetBodyColor(newColor);
		}

		ImGui::NextColumn();

		ImGui::Text(pathfinder->StateString().c_str());

		ImGui::NextColumn();

		ImGui::Text(pathfinder->Result().c_str());

		ImGui::NextColumn();
	}

	ImGui::Columns();
}

void PathfinderManager::OnRenderTargetResize(const sf::Vector2f& size)
{
	_desiredRenderTargetSize = size;
}

void PathfinderManager::Start()
{
	const auto& activeGrid = ActiveTraverseGrid();
	for (auto& pathfinder : _pathfinders)
	{
		pathfinder->Start(activeGrid->StartUid(), activeGrid->GoalUid(), activeGrid->SubGoalUids());
	}
}

void PathfinderManager::Pause()
{
	for (auto& pathfinder : _pathfinders)
	{
		pathfinder->Pause();
	}
}

void PathfinderManager::Resume()
{
	for (auto& pathfinder : _pathfinders)
	{
		pathfinder->Resume();
	}
}

void PathfinderManager::Restart()
{
	for (auto& pathfinder : _pathfinders)
	{
		pathfinder->Restart();
	}
}

void PathfinderManager::Reset()
{
	for (auto& pathfinder : _pathfinders)
	{
		pathfinder->Reset();
	}

	const auto& activeGrid = ActiveTraverseGrid();
	activeGrid->Reset();
	activeGrid->ClearObstacles();
	activeGrid->ClearSubGoals();
	activeGrid->ResetStartGoal();
}

auto PathfinderManager::Pathfinders() const -> const List<Unique<Pathfinder>>&
{
	return _pathfinders;
}

auto PathfinderManager::EditState() const -> PathfinderManagerEditState { return _editState; }

auto PathfinderManager::RunningDuration() const -> const sf::Time& { return _runningDuration; }

void PathfinderManager::SetEditState(PathfinderManagerEditState editState)
{
	_editState = editState;
}

auto PathfinderManager::ActiveTraverseGrid() -> Shared<TraverseGrid>& { return *_activeTraverseGrid; }

auto PathfinderManager::ActiveTraverseGrid() const -> const Shared<TraverseGrid>& { return *_activeTraverseGrid; }

void PathfinderManager::ClearTimerResults()
{
	_oldResults.clear();
}

void PathfinderManager::SetSleepDelay(sf::Time delay)
{
	for (auto& pathfinder : _pathfinders)
	{
		pathfinder->SetSleepDelay(delay);
	}
}

void PathfinderManager::SetWeight(int uidFirst, int uidSecond, float weight)
{
	for (auto& pathfinder : _pathfinders)
	{
		pathfinder->SetWeight(uidFirst, uidSecond, weight);
	}
	ActiveTraverseGrid()->SetWeight(uidFirst, uidSecond, weight);
}

void PathfinderManager::SetActiveTraverseGrid(const String& name)
{
	if (_activeTraverseGrid != _traverseGrids.end() && (*_activeTraverseGrid)->Name() == name)
	{
		return;
	}

	_activeTraverseGrid = SetActiveHelper(_traverseGrids, name);
	auto activeGrid = ActiveTraverseGrid();
	activeGrid->SetNoWallsToSmash(_mazeNewPaths);

	Restart();
	CollectWorker();

	_finishedWorking = false;
	_worker = Thread([this, activeGrid]
	{
		while (!_allowedToWork)
		{
		}
		activeGrid->OnRenderTargetResize(_renderTargetSize);
		_allowedToWork = false;
		_didOnFinishWorkingUpdate = false;
		_finishedWorking = true;
	});
}

auto PathfinderManager::ActivePathfinders() -> List<List<Unique<Pathfinder>>::iterator>
{
	List<List<Unique<Pathfinder>>::iterator> activePathfinders;
	for (auto iter = _pathfinders.begin(); iter != _pathfinders.end(); ++iter)
	{
		if ((*iter)->Active())
		{
			activePathfinders.push_back(iter);
		}
	}
	return activePathfinders;
}

void PathfinderManager::CollectWorker()
{
	_allowedToWork = true;
	if (_worker.joinable())
	{
		_worker.join();
	}
}
}
