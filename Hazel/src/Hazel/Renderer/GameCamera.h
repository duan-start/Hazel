#pragma once

#include <glm/glm.hpp>

namespace Hazel {

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
