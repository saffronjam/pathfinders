#pragma once

#include "TraverseGrid.h"

namespace Se
{
class VoronoiGrid : public TraverseGrid
{
public:
	VoronoiGrid();

	void OnRender(Scene &scene) override;

private:
	void GenerateNodes() override;
	void GenerateGrid() override;
	void CalculateNeighbors() override;

	void ClearNodeColor(int uid) override;
	void SetNodeColor(int uid, const sf::Color &color) override;

private:
	Voronoi _grid;
	int noPoints = 1600;
};
}
