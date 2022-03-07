#include "ProjectLayer.h"

namespace Se
{
void ProjectLayer::OnAttach(std::shared_ptr<Batch>& batch)
{
	BaseLayer::OnAttach(batch);

	batch->Submit([this]
	{
		_pathfinderManager = std::make_shared<PathfinderManager>();
		_pathfinderManager->OnUpdate(_scene);
		_pathfinderManager->OnRender(_scene);
	}, "Creating Pathfinder Manager");
}

void ProjectLayer::OnDetach()
{
	_pathfinderManager->OnExit();
	BaseLayer::OnDetach();
}

void ProjectLayer::OnUpdate()
{
	BaseLayer::OnUpdate();

	_pathfinderManager->OnUpdate(_scene);
	_pathfinderManager->OnRender(_scene);
}

void ProjectLayer::OnGuiRender()
{
	BaseLayer::OnGuiRender();

	if (ImGui::Begin("Project"))
	{
		_pathfinderManager->OnGuiRender();
	}
	ImGui::End();
}

void ProjectLayer::OnRenderTargetResize(const sf::Vector2f& newSize)
{
	BaseLayer::OnRenderTargetResize(newSize);
	_scene.OnRenderTargetResize(newSize);

	_pathfinderManager->OnRenderTargetResize(newSize);
}
}
