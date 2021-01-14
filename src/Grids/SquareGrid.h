#pragma once

#include <SFML/Graphics/RectangleShape.hpp>

#include "TraverseGrid.h"

namespace Se
{
class SquareGrid : public TraverseGrid
{
private:
	struct Square
	{
		size_t VAIndex;
		sf::FloatRect Shape;
	};

public:
	SquareGrid();

	void OnRender(Scene &scene) override;
	void OnRenderTargetResize(const sf::Vector2f &size) override;

	void ClearNodeColor(int uid) override;
	void SetNodeColor(int uid, const sf::Color &color) override;

private:
	void GenerateNodes() override;
	void GenerateGrid() override;
	void CalculateNeighbors() override;

	sf::Vector2f GetBoxSize() const;


private:
	Map<int, Square> _filledSquares;

	sf::Vector2i _noBoxes;

	sf::VertexArray _lineVA;
	sf::VertexArray _filledSquaresVA;
};
}
