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

	_drawFlags &TraverseGridDrawFlag_Grid ? _grid.ShowGrid() : _grid.HideGrid();
	_drawFlags &TraverseGridDrawFlag_Objects ? _grid.ShowFilled() : _grid.HideFilled();
	scene.Submit(_grid);
	if ( _drawFlags & TraverseGridDrawFlag_Objects )
	{
		scene.Submit(_filledEdgesVA);
	}
}

void VoronoiGrid::OnRenderTargetResize(const sf::Vector2f &size)
{
	_noPoints = (size.x / 15.0f) * (size.y / 25.0f);

	TraverseGrid::OnRenderTargetResize(size);
}

void VoronoiGrid::ClearNodeColor(int uid)
{
	Debug::Assert(uid != -1, "Invalid uid");
	_grid.PolygonAt(NodeByUid(uid).Position()).SetFillColor(sf::Color::Transparent);
}

void VoronoiGrid::SetNodeColor(int uid, const sf::Color &color)
{
	Debug::Assert(uid != -1, "Invalid uid");
	_grid.PolygonAt(NodeByUid(uid).Position()).SetFillColor(color);
}

void VoronoiGrid::ClearNodeEdgeColor(int fromUid, int toUid)
{
	Debug::Assert(fromUid != -1 && toUid != -1, "Invalid uid");
	const auto findResult = _filledEdges.find({ fromUid, toUid });
	if ( findResult != _filledEdges.end() )
	{
		const int VAIndex = findResult->second;
		_filledEdgesVA[VAIndex].color = sf::Color::Transparent;
		_filledEdgesVA[VAIndex + 1].color = sf::Color::Transparent;
		_filledEdgesVA[VAIndex + 2].color = sf::Color::Transparent;
		_filledEdgesVA[VAIndex + 3].color = sf::Color::Transparent;
	}
}

void VoronoiGrid::SetNodeEdgeColor(int fromUid, int toUid, const sf::Color &color)
{
	Debug::Assert(fromUid != -1 && toUid != -1, "Invalid uid");
	if ( _filledEdges.find({ fromUid, toUid }) == _filledEdges.end() )
	{
		const auto &firstPosition = NodeByUid(fromUid).Position();
		const auto &secondPosition = NodeByUid(toUid).Position();

		const auto firstPolygon = _grid.PolygonAt(firstPosition);
		const auto secondPolygon = _grid.PolygonAt(secondPosition);

		const auto firstPolygonMid = GenUtils::Mid(firstPolygon.Points());
		const auto secondPolygonMid = GenUtils::Mid(secondPolygon.Points());

		const auto closestEdge = firstPolygon.ClosestEdge(firstPolygonMid + (secondPolygonMid - firstPolygonMid) / 2.0f);
		const auto leftToRightNorm = VecUtils::Unit(closestEdge.first - closestEdge.second);

		const auto perpendicularLeftToRightNorm = VecUtils::Perpendicular(leftToRightNorm);
		const float thickness = _visRect.width / 1200.0f;

		const auto VAIndex = _filledEdgesVA.getVertexCount();

		_filledEdgesVA.append({ closestEdge.first + perpendicularLeftToRightNorm * thickness,color });
		_filledEdgesVA.append({ closestEdge.first - perpendicularLeftToRightNorm * thickness, color });
		_filledEdgesVA.append({ closestEdge.second - perpendicularLeftToRightNorm * thickness , color });
		_filledEdgesVA.append({ closestEdge.second + perpendicularLeftToRightNorm * thickness , color });

		_filledEdges.emplace(CreatePair(fromUid, toUid), VAIndex);
	}
	else
	{
		const int VAIndex = _filledEdges.at({ fromUid, toUid });
		_filledEdgesVA[VAIndex].color = color;
		_filledEdgesVA[VAIndex + 1].color = color;
		_filledEdgesVA[VAIndex + 2].color = color;
		_filledEdgesVA[VAIndex + 3].color = color;
	}
}

void VoronoiGrid::GenerateGrid()
{
	_grid.SetBoundingBox(_visRect);
	_grid.SetPoints(_noPoints);
	_grid.ForceGenerate();
	_grid.Relax(_noRelaxIterations);
}

void VoronoiGrid::GenerateNodes()
{
	_nodes.clear();
	int uid = 0;
	const auto &polygons = _grid.Polygons();
	for ( const auto &polygon : polygons )
	{
		_nodes.emplace(uid++, Node(uid, GenUtils::Mid(polygon.Points())));
	}
	_grid.SetOutlineColor(_gridColor);
}

void VoronoiGrid::CalculateNeighbors()
{
	const auto &polygons = _grid.Polygons();
	for ( size_t i = 0; i < polygons.size(); i++ )
	{
		const auto &neighbors = polygons[i].Neighbors();
		for ( const auto &neighbor : neighbors )
		{
			const int neighborUID = NodeUidByPosition(GenUtils::Mid(neighbor->Points()));
			const float cost = VecUtils::Length(GenUtils::Mid(polygons[i].Points()) - NodeByUid(neighborUID).Position());
			NodeByUid(i).AddNeighbor(neighborUID, cost);
		}
	}
}
}
