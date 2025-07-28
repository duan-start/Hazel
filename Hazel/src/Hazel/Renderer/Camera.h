#pragma once

#include <glm/glm.hpp>

namespace Hazel {
	//这里暂时就不用了，后期添加3d系统的时候可以把这里一块加上
	enum class CameraAPI {
		None=0,Ortho=1, Perspective =2,
	};

	class Camera
	{
	public:
		struct OrthographicSpec {
			float Left, Right, Bottom, Top;
			OrthographicSpec(const glm::vec4& screen):
				Left(screen.x),Right(screen.y),Bottom(screen.z),Top(screen.w)
			{}
		};

		struct PerspectiveSpec {
			float Fov, AspectRatio, Near, Far;
			PerspectiveSpec(const glm::vec4& screen) :
				Fov(screen.x), AspectRatio(screen.y), Near(screen.z), Far(screen.w)
			{
			}
		};

		struct CameraSpec {
			CameraAPI Type;
			union {
				OrthographicSpec ortho;
				PerspectiveSpec perspective;
			};

			CameraSpec(CameraAPI type, const glm::vec4& screen) :
				Type(type), ortho(screen)
			{

			}
		};
	public:

		virtual const glm::vec3& GetPosition()const = 0;
		virtual const glm::vec4& GetRotation()const = 0;

		virtual void SetPosition(const glm::vec3& position) = 0;
		virtual void SetRotation(const glm::vec4& rotation)  = 0;
		virtual void SetProjection(const glm::vec4& screen) = 0;

		virtual const glm::mat4& GetViewMatrix() const = 0;
		virtual const glm::mat4& GetProjectionMatrix() const = 0;
		virtual const glm::mat4& GetViewProjectionMatrix() const = 0;

		static Ref<Camera> Create(const CameraSpec& spec);
	protected:
		virtual void RecalculateViewMatrix()  = 0;
	private:
		static CameraAPI s_CamerType;
	};
}


