#include "SaffronPCH.h"

#define JC_VORONOI_IMPLEMENTATION

#include "Saffron/Core/Voronoi.h"

namespace Se
{

Voronoi::Voronoi() :
	Voronoi(sf::FloatRect(0.0f, 0.0f, 0.0f, 0.0f), ArrayList<sf::Vector2f>())
{
}

Voronoi::Voronoi(const sf::FloatRect &boundingBox) :
	_boundingBox(boundingBox),
	_fillColors({ sf::Color::Transparent })
{
}

Voronoi::Voronoi(const sf::FloatRect &boundingBox, ArrayList<sf::Vector2f> points) :
	_boundingBox(boundingBox),
	_points(Move(points)),
	_fillColors({ sf::Color::Transparent })
{
	GenerateVoronoi();
}

Voronoi::Voronoi(const sf::FloatRect &boundingBox, int noRandomPoints) :
	_boundingBox(boundingBox),
	_fillColors({ sf::Color::Transparent })
{
	GeneratePoints(noRandomPoints);
	GenerateVoronoi();
}

Voronoi::~Voronoi()
{
	if ( _diagram.has_value() )
	{
		jcv_diagram_free(&_diagram.value());
	}
}

void Voronoi::SetPoints(const ArrayList<sf::Vector2f> &points)
{
	_points = points;
	GenerateVoronoi();
}

void Voronoi::SetPoints(int noRandomPoints)
{
	GeneratePoints(noRandomPoints);
	GenerateVoronoi();
}

void Voronoi::SetBoundingBox(const sf::FloatRect &boundingBox)
{
	_boundingBox = boundingBox;
	GenerateVoronoi();
}

void Voronoi::SetOutlineColor(const sf::Color &color)
{
	for ( auto &polygon : _polygons )
	{
		polygon.setOutlineColor(color);
	}
}

void Voronoi::SetOutlineThickness(float thickness)
{
	for ( auto &polygon : _polygons )
	{
		polygon.setOutlineThickness(thickness);
	}
}

void Voronoi::Relax(int iterations)
{
	SE_CORE_ASSERT(_diagram.has_value(), "Voronoi was not created. _diagram was not instantiated");
	for ( int i = 0; i < iterations; i++ )
	{
		const jcv_site *sites = jcv_diagram_get_sites(&_diagram.value());
		for ( int i = 0; i < _diagram.value().numsites; ++i )
		{
			const jcv_site *site = &sites[i];
			jcv_point sum = site->p;
			int count = 1;
			for ( const jcv_graphedge *edge = site->edges; edge != nullptr; edge = edge->next )
			{
				sum.x += edge->pos[0].x;
				sum.y += edge->pos[0].y;
				++count;
			}
			_points[site->index].x = sum.x / count;
			_points[site->index].y = sum.y / count;
		}
	}
	GenerateVoronoi();
}

Voronoi::Polygon &Voronoi::GetPolygon(const sf::Vector2f &position)
{
	float minDistance = std::numeric_limits<float>::infinity();
	Polygon *closest = nullptr;
	for ( auto &polygon : _polygons )
	{
		const float distance = VecUtils::LengthSq(polygon.getVoronoiPoint() - position);
		if ( distance < minDistance )
		{
			minDistance = distance;
			closest = &polygon;
		}
	}

	SE_CORE_ASSERT(closest != nullptr, "Failed to find any polygons closer than std::numeric_limits<float>::infinity() units from given position");
	return *closest;
}

void Voronoi::GenerateVoronoi()
{
	if ( _boundingBox.width > 0.0f &&
		_boundingBox.height > 0.0f &&
		!_points.empty() )
	{
		jcv_rect rect = ConvertBoundingBox(_boundingBox);

		if ( _diagram.has_value() )
		{
			jcv_diagram_free(&_diagram.value());
			std::memset(&_diagram.value(), 0, sizeof(jcv_diagram));
		}
		_diagram = jcv_diagram();
		jcv_diagram_generate(_points.size(), reinterpret_cast<const jcv_point *>(_points.data()), &rect, nullptr,
							 &_diagram.value());

		// Generate polygon list from sites
		const jcv_site *sites = jcv_diagram_get_sites(&_diagram.value());
		_polygons.clear();
		_polygons.reserve(_diagram.value().numsites);
		for ( int i = 0; i < _diagram.value().numsites; ++i )
		{
			const jcv_site *site = &sites[i];

			ArrayList<sf::Vector2f> polygonPoints;
			for ( jcv_graphedge *edge = site->edges; edge != nullptr; edge = edge->next )
			{
				polygonPoints.emplace_back(edge->pos[0].x, edge->pos[0].y);
			}
			sf::ConvexShape convexShape = GenUtils::CreateConvexShape(polygonPoints);
			Polygon polygon(convexShape, VecUtils::ConvertTo<sf::Vector2f>(site->p));
			polygon.setFillColor(_fillColors[i % _fillColors.size()]);
			polygon.setOutlineColor(sf::Color::White);
			polygon.setOutlineThickness(1);

			_polygons.emplace_back(polygon);
		}

		for ( int i = 0; i < _diagram.value().numsites; ++i )
		{
			const jcv_site *site = &sites[i];

			for ( jcv_graphedge *edge = site->edges; edge != nullptr; edge = edge->next )
			{
				if ( edge->neighbor )
				{
					Voronoi::Polygon &neighbor = GetPolygon(VecUtils::ConvertTo<sf::Vector2f>(edge->neighbor->p));
					_polygons[i].addNeighbor(&neighbor);
				}
			}
		}
	}
}

jcv_rect Voronoi::ConvertBoundingBox(const sf::FloatRect &boundingBox)
{
	return { boundingBox.left, boundingBox.top, boundingBox.left + boundingBox.width,
			boundingBox.top + boundingBox.height };
}

void Voronoi::GeneratePoints(int noPoints)
{
	_points.clear();
	_points.reserve(noPoints);
	for ( int i = 0; i < noPoints; i++ )
	{
		const sf::Vector2f lowerBound(_boundingBox.left, _boundingBox.top);
		const sf::Vector2f upperBound(_boundingBox.left + _boundingBox.width, _boundingBox.top + _boundingBox.height);
		_points.push_back(Random::Vec2(lowerBound, upperBound));
	}
}

void Voronoi::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	SE_CORE_ASSERT("Voronoi was not created. _diagram was not instantiated");
	for ( auto &polygon : _polygons )
	{
		target.draw(polygon, states);
	}
}
}