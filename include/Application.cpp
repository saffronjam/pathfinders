#include "Application.hpp"

Application::Application(sf::Time &dt)
	: m_gfx(m_camera),
	  m_dt(dt),
	  m_iu(m_gfx, m_camera),
	  m_ui(m_gfx, m_iu),
	  m_camera(m_gfx, m_camera_follow, m_camera_controller),
	  m_camera_controller(m_gfx, m_camera, m_iu),
	  m_camera_follow(0.0f, 0.0f),
	  m_grid(m_iu, m_solver, sf::Vector2i(20, 20)),
	  m_solver(m_grid),
	  m_UIBackground(sf::Vector2f(197, Graphics::ScreenHeight))
{
	m_solver.Solve();
	m_ui.CreateStaticButton(0,
							std::bind(&Grid::ChangeToBoxMode, &m_grid),
							"Box M",
							sf::Vector2f((float)Graphics::ScreenWidth - 180.0f, 5.0f));
	m_ui.CreateStaticButton(1,
							std::bind(&Grid::ChangeToPolygonMode, &m_grid),
							"Poly M",
							sf::Vector2f((float)Graphics::ScreenWidth - 130.0f, 5.0f));

	m_ui.CreateStaticButton(2,
							std::bind(&AStar::ResetAll, &m_solver),
							"Reset A",
							sf::Vector2f((float)Graphics::ScreenWidth - 180.0f, 25.0f));
	m_ui.CreateStaticButton(3,
							std::bind(&AStar::ResetSearch, &m_solver),
							"Reset P",
							sf::Vector2f((float)Graphics::ScreenWidth - 130.0f, 25.0f));
	m_ui.CreateStaticFloatSlider(0,
								 m_solver.GetStepTimeRef(),
								 0.0f, 2.0f, 0.2f,
								 sf::Vector2f((float)Graphics::ScreenWidth - 180.0f, 50.0f));
	m_ui.CreateStaticCheckbox(0,
							  m_solver.GetIsPausedRef(),
							  true,
							  sf::Vector2f((float)Graphics::ScreenWidth - 180.0f, 85.0f));
	m_ui.CreateStaticRadioButtonSet(0,
									{m_grid.GetIsPlacingObstaclesPtr(),
									 m_grid.GetIsRemovingObstaclesPtr(),
									 m_grid.GetIsPlacingStartPtr(),
									 m_grid.GetIsPlacingFinalPtr()},
									{false, true, false, false},
									{sf::Vector2f((float)Graphics::ScreenWidth - 160.0f, 85.0f),
									 sf::Vector2f((float)Graphics::ScreenWidth - 145.0f, 85.0f),
									 sf::Vector2f((float)Graphics::ScreenWidth - 130.0f, 85.0f),
									 sf::Vector2f((float)Graphics::ScreenWidth - 115.0f, 85.0f)},
									{});

	m_UIBackground.setPosition((float)Graphics::ScreenWidth - m_UIBackground.getSize().x, 0.0f);
	m_UIBackground.setFillColor(sf::Color(100, 100, 100, 100));
}

void Application::Go()
{
	HandleEvents();
	m_gfx.ClearFrame();
	UpdateLogic();
	RenderFrame();
	m_gfx.EndFrame();
}

void Application::HandleEvents()
{
	while (m_gfx.GetRenderWindow().pollEvent(m_event))
	{
		// Close window: exit
		if (m_event.type == sf::Event::Closed)
		{
			m_gfx.GetRenderWindow().close();
		}
		else if (m_event.type == sf::Event::MouseWheelMoved)
		{
			float delta_move = m_event.mouseWheel.delta;
			float zoom_strength = 0.03f;
			m_camera.SetZoom(m_camera.GetZoom() + sf::Vector2f(delta_move, delta_move) * zoom_strength);
		}
	}
}

void Application::UpdateLogic()
{
	m_iu.Update();
	m_camera_controller.Update(m_dt);
	m_camera.Update(m_dt);
	m_ui.Update(m_dt);

	m_grid.Update();
	m_solver.Update(m_dt);

	switch (m_grid.GetGridModeRef())
	{
	case Grid::GridMode::ModeBox:
		m_ui.GetButton(0)->second.SetIsActive(false);
		m_ui.GetButton(1)->second.SetIsActive(true);
		break;
	case Grid::GridMode::ModePolygon:
		m_ui.GetButton(0)->second.SetIsActive(true);
		m_ui.GetButton(1)->second.SetIsActive(false);
		break;
	}
}

void Application::RenderFrame()
{
	m_grid.DrawGrid(m_gfx);
	m_solver.DrawAllNodeViaConnections(m_gfx);
	m_solver.DrawCurrentAnticipation(m_gfx);
	m_solver.DrawFinal(m_gfx);
	m_gfx.Render(m_UIBackground);
	m_ui.Draw();
	m_iu.DrawMouseCoordinates();
}