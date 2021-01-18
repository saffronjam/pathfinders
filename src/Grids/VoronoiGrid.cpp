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
	if ( _drawFlags & DrawFlag_Objects )
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
	SE_CORE_ASSERT(uid != -1, "Invalid uid");
	_grid.GetPolygon(GetNode(uid).GetPosition()).SetFillColor(sf::Color::Transparent);
}

void VoronoiGrid::SetNodeColor(int uid, const sf::Color &color)
{
	SE_CORE_ASSERT(uid != -1, "Invalid uid");
	_grid.GetPolygon(GetNode(uid).GetPosition()).SetFillColor(color);
}

void VoronoiGrid::ClearNodeEdgeColor(int fromUid, int toUid)
{
	SE_CORE_ASSERT(fromUid != -1 && toUid != -1, "Invalid uid");
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
	SE_CORE_ASSERT(fromUid != -1 && toUid != -1, "Invalid uid");
	if ( _filledEdges.find({ fromUid, toUid }) == _filledEdges.end() )
	{
		const auto &firstPosition = GetNode(fromUid).GetPosition();
		const auto &secondPosition = GetNode(toUid).GetPosition();

		const auto firstPolygon = _grid.GetPolygon(firstPosition);
		const auto secondPolygon = _grid.GetPolygon(secondPosition);

		const auto firstPolygonMid = GenUtils::Mid(firstPolygon.GetPoints());
		const auto secondPolygonMid = GenUtils::Mid(secondPolygon.GetPoints());

		const auto closestEdge = firstPolygon.GetClosestEdge(firstPolygonMid + (secondPolygonMid - firstPolygonMid) / 2.0f);
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
