#include "Grids/SquareGrid.h"

namespace Se
{
SquareGrid::SquareGrid() :
	TraverseGrid("Square"),
	_noBoxes(50, 50),
	_lineVA(sf::PrimitiveType::Lines),
	_filledSquaresVA(sf::PrimitiveType::Quads)
{
	SquareGrid::GenerateGrid();
	SquareGrid::GenerateNodes();
	SquareGrid::CalculateNeighbors();
}

void SquareGrid::OnRender(Scene &scene)
{
	TraverseGrid::OnRender(scene);

	if ( _drawFlags & DrawFlag_Grid )
	{
		scene.Submit(_lineVA);
	}

	if ( _drawFlags & DrawFlag_Objects )
	{
		scene.Submit(_filledSquaresVA);
	}
}

void SquareGrid::OnRenderTargetResize(const sf::Vector2f &size)
{
	const bool newSize = _renderTargetSize != size;
	if ( newSize )
	{
		const int boxesX = static_cast<int>(size.x) / 12;
		const int boxesY = std::round(size.y / size.x * static_cast<float>(boxesX));
		_noBoxes = { boxesX, boxesY };
		_filledSquares.clear();
		_filledSquaresVA.clear();
	}

	TraverseGrid::OnRenderTargetResize(size);
}

void SquareGrid::ClearNodeColor(int uid)
{
	SE_CORE_ASSERT(uid != -1, "Invalid uid");
	const auto findResult = _filledSquares.find(uid);
	if ( findResult != _filledSquares.end() )
	{
		_filledSquaresVA[findResult->second.VAIndex].color = sf::Color::Transparent;
		_filledSquaresVA[findResult->second.VAIndex + 1].color = sf::Color::Transparent;
		_filledSquaresVA[findResult->second.VAIndex + 2].color = sf::Color::Transparent;
		_filledSquaresVA[findResult->second.VAIndex + 3].color = sf::Color::Transparent;
	}
}

void SquareGrid::SetNodeColor(int uid, const sf::Color &color)
{
	SE_CORE_ASSERT(uid != -1, "Invalid uid");
	if ( _filledSquares.find(uid) == _filledSquares.end() )
	{
		const auto &position = GetNode(uid).GetPosition();
		const auto boxSize = GetBoxSize();
		const auto halfBoxSize = boxSize / 2.0f;

		const auto square = Square{ _filledSquaresVA.getVertexCount(), sf::FloatRect{ position, boxSize } };
		const auto &[index, shape] = square;

		_filledSquaresVA.append({ sf::Vector2f{ shape.left, shape.top } - halfBoxSize, color });
		_filledSquaresVA.append({ sf::Vector2f{ shape.left + boxSize.x, shape.top } - halfBoxSize, color });
		_filledSquaresVA.append({ sf::Vector2f{ shape.left + boxSize.x, shape.top + boxSize.y } - halfBoxSize, color });
		_filledSquaresVA.append({ sf::Vector2f{ shape.left, shape.top + boxSize.y } - halfBoxSize, color });

		_filledSquares.emplace(uid, square);
	}
	else
	{
		_filledSquaresVA[_filledSquares.at(uid).VAIndex].color = color;
		_filledSquaresVA[_filledSquares.at(uid).VAIndex + 1].color = color;
		_filledSquaresVA[_filledSquares.at(uid).VAIndex + 2].color = color;
		_filledSquaresVA[_filledSquares.at(uid).VAIndex + 3].color = color;
	}
}

void SquareGrid::GenerateGrid()
{
	const sf::Vector2f topLeft = _visRect.getPosition();
	const auto boxSize = GetBoxSize();
	const sf::Vector2f lineLength = _visRect.getSize();


	auto setupVertex = [this](int index, const sf::Vector2f &start, const sf::Vector2f &end)
	{
		_lineVA[index].position = start;
		_lineVA[index + 1].position = end;
		_lineVA[index].color = _gridColor;
		_lineVA[index + 1].color = _gridColor;
	};

	// Setup new line vertex array
	const sf::Vector2i noVertices(2 * _noBoxes.x + 2, 2 * _noBoxes.y + 2);
	_lineVA.clear();
	_lineVA.resize(noVertices.x + noVertices.y);

	for ( int i = 0; i < noVertices.x; i += 2 )
	{
		const sf::Vector2f start = topLeft + sf::Vector2f(static_cast<float>(i / 2) * boxSize.x, 0.0f);
		const sf::Vector2f end(start.x, start.y + lineLength.y);
		setupVertex(i, start, end);
	}

	for ( int i = 0; i < noVertices.y; i += 2 )
	{
		const sf::Vector2f start = topLeft + sf::Vector2f(0.0f, static_cast<float>(i / 2) * boxSize.y);
		const sf::Vector2f end(start.x + lineLength.x, start.y);
		setupVertex(i + noVertices.x, start, end);
	}
}

void SquareGrid::GenerateNodes()
{
	_nodes.clear();

	int uid = 0;
	const auto boxSize = GetBoxSize();
	const auto topLeft = sf::Vector2f(_visRect.left, _visRect.top) + sf::Vector2f(boxSize.x, boxSize.y) / 2.0f;

	for ( int i = 0; i < _noBoxes.y; i++ )
	{
		for ( int j = 0; j < _noBoxes.x; j++ )
		{
			const sf::Vector2f position(topLeft.x + j * boxSize.x, topLeft.y + i * boxSize.y);
			_nodes.emplace(uid++, Node(uid, position));
		}
	}
}

void SquareGrid::CalculateNeighbors()
{
	const auto boxSize = GetBoxSize();
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

sf::Vector2f SquareGrid::GetBoxSize() const
{
	return { _visRect.width / static_cast<float>(_noBoxes.x), _visRect.height / static_cast<float>(_noBoxes.y) };
}

}
