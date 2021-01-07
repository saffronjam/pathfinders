#include "Grids/VoronoiGrid.h"

namespace Se
{
VoronoiGrid::VoronoiGrid() :
	TraverseGrid("Voronoi")
{
	VoronoiGrid::GenerateNodes();
	VoronoiGrid::GenerateGrid();
	VoronoiGrid::CalculateNeighbors();
}

void VoronoiGrid::OnRender(Scene &scene)
{
	scene.Submit(_grid);
}

void VoronoiGrid::GenerateNodes()
{
	_grid.SetBoundingBox(_visRect);
	_grid.SetPoints(noPoints);
}

void VoronoiGrid::GenerateGrid()
{
	int uid = 0;
	const auto &polygons = _grid.GetPolygons();
	for ( const auto &polygon : polygons )
	{
		_nodes.emplace(uid++, Node(uid, GenUtils::Mid(polygon)));
	}
	_grid.SetOutlineColor(sf::Color::Blue);

}

void VoronoiGrid::CalculateNeighbors()
{
	const auto &polygons = _grid.GetPolygons();
	for ( size_t i = 0; i < polygons.size(); i++ )
	{
		const auto &neighbors = polygons[i].getNeighbors();
		for ( const auto &neighbor : neighbors )
		{
			const int neighborUID = GetNodeUID(GenUtils::Mid(*neighbor));
			const float cost = VecUtils::Length(GenUtils::Mid(polygons[i]) - GetNode(neighborUID).GetPosition());
			GetNode(i).AddNeighbor(neighborUID, cost);
		}
	}
}

void VoronoiGrid::ClearNodeColor(int uid)
{
	SE_CORE_ASSERT(uid != -1, "Invalid uid");
	_grid.GetPolygon(GetNode(uid).GetPosition()).setFillColor(sf::Color::Transparent);
}

void VoronoiGrid::SetNodeColor(int uid, const sf::Color &color)
{
	SE_CORE_ASSERT(uid != -1, "Invalid uid");
	_grid.GetPolygon(GetNode(uid).GetPosition()).setFillColor(color);
}
}
