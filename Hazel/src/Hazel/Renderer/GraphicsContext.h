#pragma once

namespace Hazel {
	class GraphicsContext {
	public:
	//抽象的图像上下文初始化
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
	};
}