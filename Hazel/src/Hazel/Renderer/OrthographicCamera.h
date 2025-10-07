#pragma once

#include "Camera.h"

namespace Hazel {
	//这个是另一条道路，现在未被使用
	//但是我觉得这个更加合理
	class OrthographicCamera :public Camera {
	public:
		
		OrthographicCamera(float Left,float Right,float Bottom,float Top);


		virtual void SetPosition(const glm::vec3& position)override;

		virtual void SetRotation(const glm::vec4& rotation)override;

		virtual void SetProjection(const glm::vec4& screen)override ;


		virtual const glm::vec3& GetPosition()const override {	return m_Position;}
		virtual const glm::vec4& GetRotation()const override { return m_Rotation; }

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
		virtual void RecalculateViewMatrix()  override;
	private:
		//主要是这个后续经常要取出来，所以就这样写
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
	
		glm::mat4 m_ViewProjectionMatrix;

		//这里必得赋初值啊，自带的好像是随机的
		glm::vec3 m_Position = {0.,0.,0.};
		glm::vec4 m_Rotation = {0.,0.,1.,0.};
	};
}