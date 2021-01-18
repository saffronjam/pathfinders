#pragma once

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

	void ClearNodeEdgeColor(int fromUid, int toUid) override;
	void SetNodeEdgeColor(int fromUid, int toUid, const sf::Color &color) override;

private:
	void GenerateNodes() override;
	void GenerateGrid() override;
	void CalculateNeighbors() override;

	sf::Vector2f GetBoxSize() const;

private:
	Map<int, Square> _filledSquares;

	// <from, to>, VAIndex
	Map<Pair<int, int>, int> _filledEdges;

	sf::Vector2i _noBoxes;

	sf::VertexArray _lineVA{ sf::PrimitiveType::Lines };
	sf::VertexArray _filledSquaresVA{ sf::PrimitiveType::Quads };

	sf::VertexArray _filledEdgesVA{ sf::PrimitiveType::Quads };
};
}
