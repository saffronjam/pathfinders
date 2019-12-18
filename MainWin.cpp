#include "SFML/System/Clock.hpp"
#include "include/Application.hpp"

#include "include/Random.hpp"

int main()
{
	sf::Clock m_application_clock;
	sf::Time m_dt;

	Application m_application(m_dt);

	Graphics *m_gfx = &m_application.m_gfx;

	while (m_gfx->GetRenderWindow().isOpen())
	{
		m_application.Go();
		m_dt = m_application_clock.getElapsedTime();
		m_application_clock.restart();
	}
	return EXIT_SUCCESS;
}