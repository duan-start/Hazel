#pragma once

#include <glm/glm.hpp>

namespace Hazel {
	//摄像机的种类，正交和透视
	enum class CameraAPI {
		None=0,Ortho=1,Prspective=2,
	};
	//由于针对场景中物体的变换都是一致的，所以可以专门提出来放cpu计算
	//主要是ViewprojectionMatrix矩阵的计算，然后上传到GPU
	class Camera
	{
	public:
		//positon x,y,z
		virtual const glm::vec3& GetPosition()const = 0;

		virtual const glm::vec3& GetRotation()const = 0;
		//这是一种写法
		virtual void SetPosition(const glm::vec3& position) = 0;
		
		//others
		virtual void SetForwardDir(const glm::vec3& forwardDir) = 0;
		virtual void SetUpDir(const glm::vec3& upDir)=0;

		//逆时针为正确的接口
		virtual void SetRotation(const glm::vec3& rotation)  = 0;

		//根据屏幕的四个参数，构建mat4*4的projection函数
		virtual void SetProjection(const glm::vec4& screen) = 0;

		virtual const glm::mat4& GetViewMatrix() const = 0;
		virtual const glm::mat4& GetProjectionMatrix() const = 0;
		virtual const glm::mat4& GetViewProjectionMatrix() const = 0;

		static Camera* CreateOrtho(float Left,float Right,float Bottom,float Top);
		static Camera* CreatePerspective(float fov, float aspect, float znear, float zfar);

	protected:
		virtual void RecalculateViewMatrix()  = 0;
	private:
		//设置的种类
		static CameraAPI s_CamerType;
	};
}


