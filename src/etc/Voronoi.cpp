#define JC_VORONOI_IMPLEMENTATION

#include "Voronoi.h"

Voronoi::Voronoi()
    : Voronoi(sf::FloatRect(0.0f, 0.0f, 0.0f, 0.0f), std::vector<sf::Vector2f>())
{
}

Voronoi::Voronoi(const sf::FloatRect &boundingBox, const std::vector<sf::Vector2f> &points)
    : m_boundingBox(boundingBox),
      m_points(m_points),
      m_fillColors({sf::Color::Red, sf::Color::Green, sf::Color::Blue}),
      m_outlineColor(sf::Color::White)
{
    GenerateVoronoi();
}

Voronoi::Voronoi(const sf::FloatRect &boundingBox, int nRandomPoints)
    : m_boundingBox(boundingBox),
      m_fillColors({sf::Color::Red, sf::Color::Green, sf::Color::Blue}),
      m_outlineColor(sf::Color::White)
{
    for (int i = 0; i < nRandomPoints; i++)
        m_points.push_back(Random::Vec2(boundingBox.left, boundingBox.top, boundingBox.left + boundingBox.width, boundingBox.top + boundingBox.height));
    GenerateVoronoi();
}

Voronoi::~Voronoi()
{
    if (m_diagram.has_value())
        jcv_diagram_free(&m_diagram.value());
}

void Voronoi::SetPoints(const std::vector<sf::Vector2f> &points)
{
    m_points = points;
    GenerateVoronoi();
}

void Voronoi::SetBoundingBox(const sf::FloatRect &boundingBox)
{
    m_boundingBox = boundingBox;
    GenerateVoronoi();
}

void Voronoi::Relax(int iterations)
{
    if (m_diagram.has_value())
    {
        for (int i = 0; i < iterations; i++)
        {
            const jcv_site *sites = jcv_diagram_get_sites(&m_diagram.value());
            for (int i = 0; i < m_diagram.value().numsites; ++i)
            {
                const jcv_site *site = &sites[i];
                jcv_point sum = site->p;
                int count = 1;
                for (const jcv_graphedge *edge = site->edges; edge != nullptr; edge = edge->next)
                {
                    sum.x += edge->pos[0].x;
                    sum.y += edge->pos[0].y;
                    ++count;
                }
                m_points[site->index].x = sum.x / count;
                m_points[site->index].y = sum.y / count;
            }
        }
        GenerateVoronoi();
    }
    else
    {
        throw Voronoi::Exception(__LINE__, __FILE__, "Voronoi was not created. m_diagram was not instantiated");
    }
}

void Voronoi::GenerateVoronoi()
{
    if (m_boundingBox.width > 0.0f &&
        m_boundingBox.height > 0.0f &&
        m_points.size() > 0)
    {
        jcv_rect rect = ConvertBoundingBox(m_boundingBox);

        if (m_diagram.has_value())
        {
            jcv_diagram_free(&m_diagram.value());
            std::memset(&m_diagram.value(), 0, sizeof(jcv_diagram));
        }
        m_diagram = jcv_diagram();
        jcv_diagram_generate(m_points.size(), reinterpret_cast<const jcv_point *>(m_points.data()), &rect, nullptr, &m_diagram.value());
    }
}

jcv_rect Voronoi::ConvertBoundingBox(const sf::FloatRect &boundingBox)
{
    return {boundingBox.left, boundingBox.top, boundingBox.left + boundingBox.width, boundingBox.top + boundingBox.height};
}

void Voronoi::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    if (m_diagram.has_value())
    {
        sf::VertexArray triangle(sf::PrimitiveType::Triangles, 3);
        const jcv_site *sites = jcv_diagram_get_sites(&m_diagram.value());
        for (int i = 0; i < m_diagram.value().numsites; ++i)
        {
            const jcv_site *site = &sites[i];

            for (jcv_graphedge *edge = site->edges; edge != nullptr; edge = edge->next)
            {
                triangle[0].position = vl::ConvertTo<sf::Vector2f>(site->p);
                triangle[1].position = vl::ConvertTo<sf::Vector2f>(edge->pos[0]);
                triangle[2].position = vl::ConvertTo<sf::Vector2f>(edge->pos[1]);
                triangle[0].color = sf::Color::Blue;
                triangle[1].color = sf::Color::Blue;
                triangle[2].color = sf::Color::Blue;
                Window::GetSFWindow()->draw(triangle, states);
            }
        }
        if (m_outlineColor != sf::Color::Transparent)
        {
            sf::VertexArray line(sf::PrimitiveType::Lines, 2);
            for (const jcv_edge *edge = jcv_diagram_get_edges(&m_diagram.value()); edge != nullptr; edge = jcv_diagram_get_next_edge(edge))
            {
                sf::Vector2f first(edge->pos[0].x, edge->pos[0].y);
                sf::Vector2f second(edge->pos[1].x, edge->pos[1].y);
                line[0].position = first;
                line[1].position = second;
                line[0].color = m_outlineColor;
                line[1].color = m_outlineColor;
                Window::GetSFWindow()->draw(line, states);
            }
        }
    }
    else
    {
        throw Voronoi::Exception(__LINE__, __FILE__, "Voronoi was not created. m_diagram was not instantiated");
    }
}

Voronoi::Exception::Exception(int line, const char *file, const char *errorString) noexcept
    : IException(line, file),
      errorString(errorString)
{
}

const char *Voronoi::Exception::what() const noexcept
{
    std::ostringstream oss;
    oss << "[Type] " << GetType() << std::endl
        << "[Description] " << GetErrorString() << std::endl
        << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char *Voronoi::Exception::GetType() const noexcept
{
    return "V-2DFramework Voronoi Exception";
}

const char *Voronoi::Exception::GetErrorString() const noexcept
{
    return errorString.c_str();
}