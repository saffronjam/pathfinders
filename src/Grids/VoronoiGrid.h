#pragma once

#include "TraverseGrid.h"

namespace Se
{
class VoronoiGrid : public TraverseGrid
{
public:
	VoronoiGrid();

	void OnRender(Scene &scene) override;
	void OnRenderTargetResize(const sf::Vector2f &size) override;

	void ClearNodeColor(int uid) override;
	void SetNodeColor(int uid, const sf::Color &color) override;

	void ClearNodeEdgeColor(int fromUid, int toUid) override;
	void SetNodeEdgeColor(int fromUid, int toUid, const sf::Color &color) override;

private:
	void GenerateNodes() override;
	void GenerateGrid() override;
	void CalculateNeighbors() override;

private:
	Voronoi _grid;
	int _noPoints = 10;
	int _noRelaxIterations = 4;

	// <from, to>, VAIndex
	Map<Pair<int, int>, int> _filledEdges;
	sf::VertexArray _filledEdgesVA{ sf::PrimitiveType::Quads };
};
}
