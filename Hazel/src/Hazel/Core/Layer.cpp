#include "hzpch.h"
#include "Layer.h"

namespace Hazel {
	Layer::Layer(const std::string& DebugName) :
		m_DebugName(DebugName) {}

	Layer::~Layer()
	{
	}

	void Layer::OnAttach()
	{
	}

	void Layer::OnDetach()
	{
	}

	void Layer::OnUpdate(Timestep ts)
	{
	}

	void Layer::OnImGuiRender()
	{
	}

	void Layer::OnEvent(Event& event)
	{
	}
}