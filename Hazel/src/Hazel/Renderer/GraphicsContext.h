#pragma once

namespace Hazel {
	class GraphicsContext {
	public:
	//�����ͼ�������ĳ�ʼ��
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
	};
}