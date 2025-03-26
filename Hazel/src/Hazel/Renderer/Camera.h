#pragma once

#include <glm/glm.hpp>

namespace Hazel {

	enum class CameraAPI {
		None=0,Ortho=1,Prspective=2,
	};

	class Camera
	{
	public:

		virtual const glm::vec3& GetPosition()const = 0;
		virtual const glm::vec4& GetRotation()const = 0;

		virtual void SetPosition(const glm::vec3& position) = 0;
		virtual void SetRotation(const glm::vec4& rotation)  = 0;
		virtual void SetProjection(const glm::vec4& screen) = 0;

		virtual const glm::mat4& GetViewMatrix() const = 0;
		virtual const glm::mat4& GetProjectionMatrix() const = 0;
		virtual const glm::mat4& GetViewProjectionMatrix() const = 0;
		

		static Camera* Create(float Left,float Right,float Bottom,float Top,float Near=-1,float Far=1);

	protected:
		virtual void RecalculateViewMatrix()  = 0;
	private:
		static CameraAPI s_CamerType;
	};
}


