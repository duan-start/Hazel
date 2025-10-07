#pragma once

#include <glm/glm.hpp>

namespace Hazel {
	//基类的摄像机（包含的东西确实少，不过这样才符合那个设计模式，所有的子类都可以被父类来代表）
	class GameCamera
	{
	public:
		GameCamera(const glm::mat4& projeciton);
		GameCamera() = default;
		virtual ~GameCamera() = default;

		const glm::mat4& GetProjection()const  { return m_Projection; }
	protected:
		glm::mat4 m_Projection{1.0f};
	};

}
