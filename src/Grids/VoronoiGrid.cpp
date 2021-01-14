#include "Grids/VoronoiGrid.h"

namespace Se
{
VoronoiGrid::VoronoiGrid() :
	TraverseGrid("Voronoi")
{
	_grid.DisableAutomaticGeneration();
	VoronoiGrid::GenerateGrid();
	VoronoiGrid::GenerateNodes();
	VoronoiGrid::CalculateNeighbors();
}

void VoronoiGrid::OnRender(Scene &scene)
{
	TraverseGrid::OnRender(scene);

	_drawFlags &DrawFlag_Grid ? _grid.ShowGrid() : _grid.HideGrid();
	_drawFlags &DrawFlag_Objects ? _grid.ShowFilled() : _grid.HideFilled();
	scene.Submit(_grid);
}

void VoronoiGrid::OnRenderTargetResize(const sf::Vector2f &size)
{
	_noPoints = (size.x / 15.0f) * (size.y / 25.0f);

	TraverseGrid::OnRenderTargetResize(size);
}

void VoronoiGrid::ClearNodeColor(int uid)
{
	SE_CORE_ASSERT(uid != -1, "Invalid uid");
	_grid.GetPolygon(GetNode(uid).GetPosition()).SetFillColor(sf::Color::Transparent);
}

void VoronoiGrid::SetNodeColor(int uid, const sf::Color &color)
{
	SE_CORE_ASSERT(uid != -1, "Invalid uid");
	_grid.GetPolygon(GetNode(uid).GetPosition()).SetFillColor(color);
}

void VoronoiGrid::GenerateGrid()
{
	_grid.SetBoundingBox(_visRect);
	_grid.SetPoints(_noPoints);
	_grid.ForceGenerate();
	_grid.Relax(4);
}

void VoronoiGrid::GenerateNodes()
{
	_nodes.clear();
	int uid = 0;
	const auto &polygons = _grid.GetPolygons();
	for ( const auto &polygon : polygons )
	{
		_nodes.emplace(uid++, Node(uid, GenUtils::Mid(polygon.GetPoints())));
	}
	_grid.SetOutlineColor(_gridColor);
}

void VoronoiGrid::CalculateNeighbors()
{
	const auto &polygons = _grid.GetPolygons();
	for ( size_t i = 0; i < polygons.size(); i++ )
	{
		const auto &neighbors = polygons[i].GetNeighbors();
		for ( const auto &neighbor : neighbors )
		{
			const int neighborUID = GetNodeUID(GenUtils::Mid(neighbor->GetPoints()));
			const float cost = VecUtils::Length(GenUtils::Mid(polygons[i].GetPoints()) - GetNode(neighborUID).GetPosition());
			GetNode(i).AddNeighbor(neighborUID, cost);
		}
	}
}
}
