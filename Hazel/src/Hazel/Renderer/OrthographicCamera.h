#pragma once

#include "Camera.h"

namespace Hazel {
//暂时未被使用
	class OrthographicCamera :public Camera {
	public:
		
		OrthographicCamera(float Left,float Right,float Bottom,float Top);

		virtual void SetPosition(const glm::vec3& position)override;

		virtual void SetRotation(const glm::vec3& rotation)override;

		virtual void SetForwardDir(const glm::vec3& forwardDir)override;
		virtual void SetUpDir(const glm::vec3& upDir)override;

		virtual void SetProjection(const glm::vec4& screen)override ;


		virtual const glm::vec3& GetPosition()const override {	return m_Position;}
		virtual const glm::vec3& GetRotation()const override { return m_Rotation; }

		virtual const glm::mat4& GetViewMatrix() const override {
			return m_ViewMatrix;
		}
		virtual const glm::mat4& GetProjectionMatrix() const override {
			return m_ProjectionMatrix;
		}
		virtual const glm::mat4& GetViewProjectionMatrix() const override {
			return m_ViewProjectionMatrix;
		}
	private:
		//改变相机的位置才使用这个值，对于projection改变的话不需要单独调用
		virtual void RecalculateViewMatrix()  override;
	private:
		glm::mat4 m_ProjectionMatrix{};
		glm::mat4 m_ViewMatrix{};
		glm::mat4 m_ViewProjectionMatrix{};

		//这里必得赋初值啊，自带的好像是随机的
		glm::vec3 m_Position = {0.f,0.f,0.f};
		glm::vec3 m_Rotation = {0.f,0.f,0.f};

		//others
		glm::vec3 m_ForwardDir = { 0.f,0.f,-1.f};
		glm::vec3 m_UpDir = { 0.f,1.f,0.f };
	};
}