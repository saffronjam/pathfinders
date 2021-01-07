#include "Grids/SquareGrid.h"

namespace Se
{
SquareGrid::SquareGrid() :
	TraverseGrid("Square"),
	_noBoxes(50, 50)
{
	SquareGrid::GenerateNodes();
	SquareGrid::GenerateGrid();
	SquareGrid::CalculateNeighbors();
}

void SquareGrid::OnRender(Scene &scene)
{
	for ( auto &[uid, rect] : _grid )
	{
		scene.Submit(rect);
	}
}

void SquareGrid::OnRenderTargetResize(const sf::Vector2f &size)
{
	const int boxesX = static_cast<int>(size.x) / 12;
	const int boxesY = std::round(size.y / size.x * static_cast<float>(boxesX));
	_noBoxes = { boxesX, boxesY };

	TraverseGrid::OnRenderTargetResize(size);
}

void SquareGrid::GenerateNodes()
{
	_nodes.clear();

	int uid = 0;
	const sf::Vector2f boxSize(_visRect.width / _noBoxes.x, _visRect.height / _noBoxes.y);
	const sf::Vector2f topLeft = sf::Vector2f(_visRect.left, _visRect.top) + sf::Vector2f(boxSize.x, boxSize.y) / 2.0f;

	for ( int i = 0; i < _noBoxes.y; i++ )
	{
		for ( int j = 0; j < _noBoxes.x; j++ )
		{
			const sf::Vector2f position(topLeft.x + j * boxSize.x, topLeft.y + i * boxSize.y);
			_nodes.emplace(CreatePair(uid++, Node(uid, position)));
		}
	}
}

void SquareGrid::GenerateGrid()
{
	_grid.clear();

	const sf::Vector2f boxSize(_visRect.width / _noBoxes.x, _visRect.height / _noBoxes.y);

	sf::RectangleShape rect;
	rect.setSize(sf::Vector2f(boxSize.x, boxSize.y));
	rect.setFillColor(sf::Color::Transparent);
	rect.setOutlineColor(sf::Color::Blue);
	rect.setOutlineThickness(1.0f);
	for ( auto &[uid, node] : _nodes )
	{
		rect.setPosition(node.GetPosition() - rect.getSize() / 2.0f);
		_grid.emplace(uid, rect);
	}
}

void SquareGrid::CalculateNeighbors()
{
	const sf::Vector2f boxSize(_visRect.width / _noBoxes.x, _visRect.height / _noBoxes.y);
	const float diagonalLength = VecUtils::Length(boxSize);
	for ( int i = 0; i < _noBoxes.x * _noBoxes.y; i++ )
	{
		for ( int j = 0; j < 8; j++ )
		{
			if ( ((i % _noBoxes.x == 0) && j == 0) ||
				((i % _noBoxes.x == 0 || (i >= 0 && i < _noBoxes.x)) && j == 1) ||
				((i >= 0 && i < _noBoxes.x) && j == 2) ||
				(((i >= 0 && i < _noBoxes.x) || (i + 1) % _noBoxes.x == 0) && j == 3) ||
				(((i + 1) % _noBoxes.x == 0) && j == 4) ||
				(((i + 1) % _noBoxes.x == 0 || i >= _noBoxes.x * (_noBoxes.y - 1)) && j == 5) ||
				((i >= _noBoxes.x * (_noBoxes.y - 1) && (i <= (_noBoxes.x * _noBoxes.y))) && j == 6) ||
				((i >= _noBoxes.x * (_noBoxes.y - 1) || i % _noBoxes.x == 0) && j == 7) )
			{
				continue;
			}
			switch ( j )
			{
			case 0:
				GetNode(i).AddNeighbor(i - 1, boxSize.x);
				break;
			case 1:
				GetNode(i).AddNeighbor(i - 1 - _noBoxes.x, diagonalLength);
				break;
			case 2:
				GetNode(i).AddNeighbor(i - _noBoxes.x, boxSize.y);
				break;
			case 3:
				GetNode(i).AddNeighbor(i + 1 - _noBoxes.x, diagonalLength);
				break;
			case 4:
				GetNode(i).AddNeighbor(i + 1, boxSize.x);
				break;
			case 5:
				GetNode(i).AddNeighbor(i + 1 + _noBoxes.x, diagonalLength);
				break;
			case 6:
				GetNode(i).AddNeighbor(i + _noBoxes.x, boxSize.y);
				break;
			case 7:
				GetNode(i).AddNeighbor(i - 1 + _noBoxes.x, diagonalLength);
				break;
			default:
				break;
			}
		}
	}
}

void SquareGrid::ClearNodeColor(int uid)
{
	SE_CORE_ASSERT(uid != -1, "Invalid uid");
	_grid.at(uid).setFillColor(sf::Color::Transparent);
}

void SquareGrid::SetNodeColor(int uid, const sf::Color &color)
{
	SE_CORE_ASSERT(uid != -1, "Invalid uid");
	_grid.at(uid).setFillColor(color);
}
}
