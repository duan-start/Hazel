#pragma once

#include <glm/glm.hpp>

namespace Hazel {
	//这里暂时就不用了，后期添加3d系统的时候可以把这里一块加上，
	//摄像机的种类，正交和透视
	enum class CameraAPI {
		None=0,Ortho=1,Prspective=2,
	};
	// 这个摄像机同样现在也没用到
	//摄像机，他的存在是将gpu的上的重复计算转到cpu的一次计算
	//主要是ViewprojectionMatrix矩阵的计算，然后上传到GPU
	class Camera
	{
	public:
		//positon x,y,z
		virtual const glm::vec3& GetPosition()const = 0;
		//vec4 四元数（没有万向锁）；vec3 (yaw,pitch,roll)会有万向锁
		//我的这个实际上是绕某个轴旋转多少而已
		virtual const glm::vec4& GetRotation()const = 0;

		virtual void SetPosition(const glm::vec3& position) = 0;
		virtual void SetRotation(const glm::vec4& rotation)  = 0;
		//根据屏幕的四个参数，构建mat4*4的projection函数
		virtual void SetProjection(const glm::vec4& screen) = 0;

		virtual const glm::mat4& GetViewMatrix() const = 0;
		virtual const glm::mat4& GetProjectionMatrix() const = 0;
		virtual const glm::mat4& GetViewProjectionMatrix() const = 0;

		static Camera* Create(float Left,float Right,float Bottom,float Top);

	protected:
		virtual void RecalculateViewMatrix()  = 0;
	private:
		//设置的种类
		static CameraAPI s_CamerType;
	};
}


