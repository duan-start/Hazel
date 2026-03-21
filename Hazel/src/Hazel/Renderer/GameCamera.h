#pragma once

#include <glm/glm.hpp>

namespace Hazel {
	//基类的摄像机（包含的东西确实少，不过这样才符合那个设计模式，所有的子类都可以被父类来代表）
	//所有的类型全部写到一个类里面，这也是ai推崇的做法，而不是像以前正交和透视分离
	//ESC的思想
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
