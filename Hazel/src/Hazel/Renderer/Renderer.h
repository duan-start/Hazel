#pragma once

namespace Hazel {
	enum class RendererAPI {
		none = 0,
		OpenGL = 1
	};
	class Renderer {
	public:
		inline static RendererAPI GetAPI() { return m_RendererAPI; }
	private:
		//static 的数据之只能类外定义
		static RendererAPI m_RendererAPI;
	};
}