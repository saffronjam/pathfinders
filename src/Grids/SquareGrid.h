#pragma once

#include <SFML/Graphics/RectangleShape.hpp>

#include "TraverseGrid.h"

namespace Se
{
class SquareGrid : public TraverseGrid
{
public:
	SquareGrid();

	void OnRender(Scene &scene) override;
	void OnRenderTargetResize(const sf::Vector2f &size) override;

private:
	void GenerateNodes() override;
	void GenerateGrid() override;
	void CalculateNeighbors() override;

	void ClearNodeColor(int uid) override;
	void SetNodeColor(int uid, const sf::Color &color) override;

private:
	Map<int, sf::RectangleShape> _grid;
	sf::Vector2i _noBoxes;
};
}
