#pragma once

#include <memory>
#include <optional>
#include <vector>
#include <set>
#include <cstring>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <jcv/jc_voronoi.h>

#include <Saffron.h>

namespace Se
{
class Voronoi : public sf::Drawable
{
public:
	class Polygon : public sf::ConvexShape
	{
	public:
		Polygon(const sf::Vector2f &voronoiPoint, size_t pointCount = 0)
			: sf::ConvexShape(pointCount),
			_voronoiPoint(voronoiPoint)
		{
		}
		Polygon(const sf::ConvexShape &shape, const sf::Vector2f &voronoiPoint)
			: sf::ConvexShape(shape),
			_voronoiPoint(voronoiPoint)
		{
		}

		void addNeighbor(Polygon *neighbor) { _neighbors.emplace(neighbor); }

		const sf::Vector2f &getVoronoiPoint() const { return _voronoiPoint; }
		const std::set<Polygon *> &getNeighbors() const { return _neighbors; }

		void setVoronoiPoint(const sf::Vector2f &voronoiPoint) { _voronoiPoint = voronoiPoint; }

	private:
		std::set<Polygon *> _neighbors;
		sf::Vector2f _voronoiPoint;
	};

public:
	Voronoi();
	explicit Voronoi(const sf::FloatRect &boundingBox);
	Voronoi(const sf::FloatRect &boundingBox, ArrayList<sf::Vector2f> points);
	Voronoi(const sf::FloatRect &boundingBox, int noRandomPoints);
	~Voronoi() override;

	void SetPoints(const ArrayList<sf::Vector2f> &points);
	void SetPoints(int noRandomPoints);
	void SetBoundingBox(const sf::FloatRect &boundingBox);
	void SetFillColors(const ArrayList<sf::Color> &fillColors) { _fillColors = fillColors; }
	void SetOutlineColor(const sf::Color &color);
	void SetOutlineThickness(float thickness);

	void Relax(int iterations = 1);

	const ArrayList<Voronoi::Polygon> &GetPolygons() const { return _polygons; }
	Polygon &GetPolygon(const sf::Vector2f &position);

protected:
	void GenerateVoronoi();
	static jcv_rect ConvertBoundingBox(const sf::FloatRect &boundingBox);

private:
	void GeneratePoints(int noPoints);

	void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

private:
	Optional<jcv_diagram> _diagram;
	sf::FloatRect _boundingBox;
	ArrayList<sf::Vector2f> _points;
	ArrayList<Voronoi::Polygon> _polygons;

	ArrayList<sf::Color> _fillColors;

};
}

