#include "PathfinderManager.h"

#include "Grids/SquareGrid.h"
#include "Grids/VoronoiGrid.h"

#include "Pathfinders/AStar.h"
#include "Pathfinders/Dijkstra.h"
#include "Pathfinders/BestFirstSearch.h"
#include "Pathfinders/Beam.h"
#include "Pathfinders/BFS.h"

namespace Se
{

PathfinderManager::PathfinderManager() :
	_editState(EditState::None)
{
	_pathfinders.push_back(std::make_unique<AStar>());
	_pathfinders.push_back(std::make_unique<Dijkstra>());
	_pathfinders.push_back(std::make_unique<BestFirstSearch>());
	_pathfinders.push_back(std::make_unique<Beam<32>>());
	_pathfinders.push_back(std::make_unique<Beam<512>>());
	_pathfinders.push_back(std::make_unique<BFS>());

	_traverseGrids.push_back(std::make_shared<SquareGrid>());
	_traverseGrids.push_back(std::make_shared<VoronoiGrid>());
	_activeTraverseGrid = _traverseGrids.end();

	SetActiveTraverseGrid("Square");


	const auto activeGrid = GetActiveTraverseGrid();
	for ( auto &pathfinder : _pathfinders )
	{
		pathfinder->SetTraverseGrid(activeGrid);
		pathfinder->AssignNodes(activeGrid->GetNodes());
	}

	for ( const auto &traverseGrid : _traverseGrids )
	{
		_traverseGridNames.push_back(traverseGrid->GetName().c_str());
	}

	_editStateNames.resize(static_cast<int>(EditState::Count));

	_editStateNames[static_cast<int>(EditState::None)] = "None";
	_editStateNames[static_cast<int>(EditState::Obstacles)] = "Obstacles";
	_editStateNames[static_cast<int>(EditState::SubGoal)] = "Subgoal";
	_editStateNames[static_cast<int>(EditState::Weights)] = "Weight";
	_editStateNames[static_cast<int>(EditState::Start)] = "Start";
	_editStateNames[static_cast<int>(EditState::Goal)] = "Goal";
}

PathfinderManager::~PathfinderManager()
{
	CollectWorker();
}

void PathfinderManager::OnUpdate(Scene &scene)
{
	const bool shiftDown = Keyboard::IsDown(sf::Keyboard::Key::LShift) || Keyboard::IsDown(sf::Keyboard::Key::RShift);
	const auto &activeGrid = GetActiveTraverseGrid();

	if ( _finishedWorking && !_didOnFinishWorkingUpdate )
	{
		CollectWorker();

		for ( auto &pathfinder : _pathfinders )
		{
			pathfinder->AssignNodes(activeGrid->GetNodes());
			pathfinder->SetTraverseGrid(activeGrid);
		}

		_didOnFinishWorkingUpdate = true;
	}


	if ( _drawWeights )
	{
		activeGrid->SetWeightColorAlpha(255);
		activeGrid->AddDrawFlags(TraverseGrid::DrawFlag_Weights);
	}
	else if ( _drawFadedWeights )
	{
		activeGrid->SetWeightColorAlpha(60);
		activeGrid->AddDrawFlags(TraverseGrid::DrawFlag_Weights);
	}
	else
	{
		activeGrid->RemoveDrawFlags(TraverseGrid::DrawFlag_Weights);
	}

	if ( _finishedWorking )
	{
		// Reset before check hovered
		if ( activeGrid->IsEdgeClear(_editPair.first, _editPair.second) )
		{
			activeGrid->ClearNodeEdgeColor(_editPair.first, _editPair.second);
		}

		if ( scene.GetViewportPane().IsHovered() )
		{
			const auto mouseInViewportPanePosition = scene.GetCamera().ScreenToWorld(scene.GetViewportPane().GetMousePosition());
			const int nodeUID = activeGrid->GetNodeUID(mouseInViewportPanePosition);


			_editPair.first = nodeUID;
			_editPair.second = activeGrid->GetClosestNeighborUID(nodeUID, mouseInViewportPanePosition);

			if ( activeGrid->IsEdgeClear(_editPair.first, _editPair.second) )
			{
				if ( _editState == EditState::Weights && !_weightBrushEnabled )
				{
					const auto color = TraverseGrid::GetWeightColor(_weight);
					activeGrid->SetNodeEdgeColor(_editPair.first, _editPair.second, color);
				}
				if ( _editState == EditState::Obstacles && !_obstacleBrushEnabled )
				{
					const auto color = activeGrid->GetGridColor();
					activeGrid->SetNodeEdgeColor(_editPair.first, _editPair.second, color);
				}
			}

			if ( Mouse::IsDown(sf::Mouse::Button::Left) )
			{
				switch ( _editState )
				{
				case EditState::Obstacles:
				{
					if ( _obstacleBrushEnabled )
					{
						for ( const auto &[uid, node] : activeGrid->GetNodes() )
						{
							if ( VecUtils::LengthSq(node.GetPosition() - mouseInViewportPanePosition) < std::pow(static_cast<float>(_obstacleBrushSize), 2.0f) )
							{
								for ( const auto &neighborUID : node.GetNeighbors() )
								{
									activeGrid->AddObstacle(uid, neighborUID);
								}
							}
						}
					}
					else
					{
						shiftDown ? activeGrid->RemoveObstacle(_editPair.first, _editPair.second) : activeGrid->AddObstacle(_editPair.first, _editPair.second);
					}
					break;
				}
				case EditState::Start:
				{
					activeGrid->SetStart(nodeUID);
					break;
				}
				case EditState::Goal:
				{
					activeGrid->SetGoal(nodeUID);
					break;
				}
				case EditState::SubGoal:
				{
					shiftDown ? activeGrid->RemoveSubGoal(nodeUID) : activeGrid->AddSubGoal(nodeUID);
					break;
				}
				case EditState::Weights:
				{
					const float weight = shiftDown ? 1.0f : _weight;
					if ( _weightBrushEnabled )
					{
						for ( const auto &[uid, node] : activeGrid->GetNodes() )
						{
							if ( VecUtils::LengthSq(node.GetPosition() - mouseInViewportPanePosition) < std::pow(static_cast<float>(_weightBrushSize), 2.0f) )
							{
								for ( const auto &neighborUID : node.GetNeighbors() )
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
				default:
					break;
				}

			}
		}

		GetActiveTraverseGrid()->OnUpdate();

		for ( auto &pathfinder : _pathfinders )
		{
			pathfinder->OnUpdate();
		}
	}
}

void PathfinderManager::OnRender(Scene &scene)
{
	_allowedToWork = !_finishedWorking;

	GetActiveTraverseGrid()->OnRender(scene);

	if ( !_allowedToWork )
	{
		OnRenderPathfinders(scene);

		if ( _editState == EditState::Obstacles && _obstacleBrushEnabled )
		{
			const sf::Color color(255, 0, 0, 50);
			const auto position = scene.GetCamera().ScreenToWorld(scene.GetViewportPane().GetMousePosition());
			scene.Submit(position, color, static_cast<float>(_obstacleBrushSize));
		}

		if ( _editState == EditState::Weights && _weightBrushEnabled )
		{
			const sf::Color color(0, 255, 0, 50);
			const auto position = scene.GetCamera().ScreenToWorld(scene.GetViewportPane().GetMousePosition());
			scene.Submit(position, color, static_cast<float>(_weightBrushSize));
		}
	}

	_allowedToWork = true;
}

void PathfinderManager::OnRenderPathfinders(Scene &scene)
{
	auto activePathfinder = GetActivePathfinders();

	if ( _drawNeighbors )
	{
		for ( auto &pathfinder : activePathfinder )
		{
			if ( !(*pathfinder)->IsDone() )
			{
				(*pathfinder)->OnRenderNeighbors(scene);
			}
		}
	}
	if ( _drawViaConnections )
	{
		for ( auto &pathfinder : activePathfinder )
		{
			(*pathfinder)->OnRenderViaConnections(scene);
		}
	}
	if ( _drawWorker )
	{
		for ( auto &pathfinder : activePathfinder )
		{
			(*pathfinder)->OnRenderBody(scene);
		}
	}
}

void PathfinderManager::OnGuiRender()
{
	ImGui::PushItemFlag(ImGuiItemFlags_Disabled, !_finishedWorking);

	auto activeGrid = GetActiveTraverseGrid();

	Gui::BeginPropertyGrid("Grid");
	ImGui::Text("Traverse Grid");
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);
	if ( ImGui::Combo("##Traverse Grid", &_activeTraverseGridIndex, _traverseGridNames.data(), _traverseGridNames.size()) )
	{
		SetActiveTraverseGrid(_traverseGridNames.at(_activeTraverseGridIndex));
	}
	ImGui::NextColumn();

	ImGui::Columns(3, "StartRestartReset");

	if ( ImGui::Button("Start", { ImGui::GetContentRegionAvailWidth(), 0 }) )
	{
		Start();
	}
	ImGui::NextColumn();
	if ( ImGui::Button("Restart", { ImGui::GetContentRegionAvailWidth(), 0 }) )
	{
		Restart();
	}
	ImGui::NextColumn();
	if ( ImGui::Button("Reset", { ImGui::GetContentRegionAvailWidth(), 0 }) )
	{
		Reset();
	}

	ImGui::Columns(2, "PauseResume");

	if ( ImGui::Button("Pause", { ImGui::GetContentRegionAvailWidth(), 0 }) )
	{
		Pause();
	}
	ImGui::NextColumn();
	if ( ImGui::Button("Resume", { ImGui::GetContentRegionAvailWidth(), 0 }) )
	{
		Resume();
	}
	ImGui::NextColumn();

	Gui::EndPropertyGrid();

	ImGui::Separator();

	ImGui::Columns(1, "SquareGenerateMaze");
	if ( ImGui::Button("Generate Maze", { ImGui::GetContentRegionAvailWidth(), 0 }) )
	{
		CollectWorker();
		_finishedWorking = false;
		_worker = Thread([this]
						 {
							 while ( !_allowedToWork )
							 {
							 }
							 Reset();
							 GetActiveTraverseGrid()->GenerateMaze();
							 _allowedToWork = false;
							 _didOnFinishWorkingUpdate = false;
							 _finishedWorking = true;
						 });
	}
	ImGui::Separator();

	Gui::BeginPropertyGrid("Time");
	if ( Gui::Property("Sleep delay", _sleepDelayMicroseconds, 0.0f, 1000000.0f, 1.0f,
					   Gui::PropertyFlag_Slider | Gui::PropertyFlag_Logarithmic) )
	{
		SetSleepDelay(sf::microseconds(_sleepDelayMicroseconds));
	}


	const auto drawFlags = activeGrid->GetDrawFlags();
	bool drawGrid = drawFlags & TraverseGrid::DrawFlag_Grid;
	bool drawObjects = drawFlags & TraverseGrid::DrawFlag_Objects;

	if ( Gui::Property("Grid", drawGrid) )
	{
		activeGrid->SetDrawFlags(drawGrid ? drawFlags | TraverseGrid::DrawFlag_Grid : drawFlags - TraverseGrid::DrawFlag_Grid);
	}
	if ( Gui::Property("Objects", drawObjects) )
	{
		activeGrid->SetDrawFlags(drawObjects ? drawFlags | TraverseGrid::DrawFlag_Objects : drawFlags - TraverseGrid::DrawFlag_Objects);
	}
	Gui::Property("Weights", _drawFadedWeights);

	Gui::EndPropertyGrid();

	ImGui::Separator();

	Gui::BeginPropertyGrid("Edit");

	String editText = "Edit";

	switch ( _editState )
	{
	case EditState::Obstacles:
	case EditState::SubGoal:
		editText += " (Hold shift to remove)";
		break;
	case EditState::Weights:
		editText += " (Hold shift to apply reset)";
		break;
	default:
		break;
	}

	ImGui::Text(editText.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);
	if ( ImGui::Combo("##EditState", &_editStateIndex, _editStateNames.data(), _editStateNames.size()) )
	{
		_drawWeights = static_cast<EditState>(_editStateIndex) == EditState::Weights;
		SetEditState(static_cast<EditState>(_editStateIndex));
	}

	ImGui::NextColumn();

	switch ( _editState )
	{
	case EditState::Obstacles:
		Gui::Property("Enable Brush", _obstacleBrushEnabled);
		if ( _obstacleBrushEnabled )
		{
			Gui::Property("Brush Size", _obstacleBrushSize, 1, 100, 1, Gui::PropertyFlag_Slider);
		}
		break;
	case EditState::Weights:
		Gui::Property("Weight", _weight, 0, TraverseGrid::MaxWeight, 1, Gui::PropertyFlag_Slider);
		Gui::Property("Enable Brush", _weightBrushEnabled);
		if ( _weightBrushEnabled )
		{
			Gui::Property("Brush Size", _weightBrushSize, 1, 100, 1, Gui::PropertyFlag_Slider);
		}
		break;
	default:
		break;
	}

	Gui::EndPropertyGrid();

	ImGui::Separator();


	ImGui::Columns(4, "Pathfinders");

	for ( auto &pathfinder : _pathfinders )
	{
		const String &name = pathfinder->GetName();
		ImGui::Text(name.c_str());

		ImGui::NextColumn();

		String id = "##" + name;
		bool active = pathfinder->IsActive();
		if ( ImGui::Checkbox(id.c_str(), &active) )
		{
			active ? pathfinder->Activate() : pathfinder->Deactivate();
		}

		ImGui::SameLine();

		id += "Color";
		const sf::Color body = pathfinder->GetBodyColor();
		float color[4] = {
			static_cast<float>(body.r) / 255.0f,
			static_cast<float>(body.g) / 255.0f,
			static_cast<float>(body.b) / 255.0f,
			static_cast<float>(body.a) / 255.0f
		};

		if ( ImGui::ColorEdit4(id.c_str(), color, ImGuiColorEditFlags_NoInputs) )
		{
			const sf::Color newColor(color[0] * 255,
									 color[1] * 255,
									 color[2] * 255,
									 color[3] * 255);
			pathfinder->SetBodyColor(newColor);
		}

		ImGui::NextColumn();

		ImGui::Text(pathfinder->GetStateString().c_str());

		ImGui::NextColumn();

		ImGui::Text(pathfinder->GetResult().c_str());

		ImGui::NextColumn();
	}

	ImGui::Columns();
}

void PathfinderManager::OnRenderTargetResize(const sf::Vector2f &size)
{
	if ( _renderTargetSize != size )
	{
		Reset();

		CollectWorker();

		_finishedWorking = false;
		_worker = Thread([this, size]
						 {
							 while ( !_allowedToWork )
							 {
							 }
							 GetActiveTraverseGrid()->OnRenderTargetResize(size);
							 _allowedToWork = false;
							 _didOnFinishWorkingUpdate = false;
							 _finishedWorking = true;
						 });

		_renderTargetSize = size;
	}
}

void PathfinderManager::Start()
{
	const auto &activeGrid = GetActiveTraverseGrid();
	for ( auto &pathfinder : _pathfinders )
	{
		pathfinder->Start(activeGrid->GetStartUID(), activeGrid->GetGoalUID(), activeGrid->GetSubGoalUIDs());
	}
}

void PathfinderManager::Pause()
{
	for ( auto &pathfinder : _pathfinders )
	{
		pathfinder->Pause();
	}
}

void PathfinderManager::Resume()
{
	for ( auto &pathfinder : _pathfinders )
	{
		pathfinder->Resume();
	}
}

void PathfinderManager::Restart()
{
	for ( auto &pathfinder : _pathfinders )
	{
		pathfinder->Restart();
	}
}

void PathfinderManager::Reset()
{
	for ( auto &pathfinder : _pathfinders )
	{
		pathfinder->Reset();
	}

	const auto &activeGrid = GetActiveTraverseGrid();
	activeGrid->Reset();
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

void PathfinderManager::SetWeight(int uidFirst, int uidSecond, float weight)
{
	for ( auto &pathfinder : _pathfinders )
	{
		pathfinder->SetWeight(uidFirst, uidSecond, weight);
	}
	GetActiveTraverseGrid()->SetWeight(uidFirst, uidSecond, weight);
}

void PathfinderManager::SetActiveTraverseGrid(const String &name)
{
	if ( _activeTraverseGrid != _traverseGrids.end() && (*_activeTraverseGrid)->GetName() == name )
	{
		return;
	}

	_activeTraverseGrid = SetActiveHelper(_traverseGrids, name);
	auto activeGrid = GetActiveTraverseGrid();

	Restart();
	CollectWorker();

	_finishedWorking = false;
	_worker = Thread([this, activeGrid]
					 {
						 while ( !_allowedToWork )
						 {
						 }
						 activeGrid->OnRenderTargetResize(_renderTargetSize);
						 _allowedToWork = false;
						 _didOnFinishWorkingUpdate = false;
						 _finishedWorking = true;
					 });
}

ArrayList<ArrayList<Unique<Pathfinder>>::iterator> PathfinderManager::GetActivePathfinders()
{
	ArrayList<ArrayList<Unique<Pathfinder>>::iterator> activePathfinders;
	for ( auto iter = _pathfinders.begin(); iter != _pathfinders.end(); ++iter )
	{
		if ( (*iter)->IsActive() )
		{
			activePathfinders.push_back(iter);
		}
	}
	return activePathfinders;
}

void PathfinderManager::CollectWorker()
{
	_allowedToWork = true;
	if ( _worker.joinable() ) _worker.join();
}
}
