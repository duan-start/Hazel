#pragma once

#include "Camera.h"

namespace Hazel {
	class PerspectivegraphicCamera : public Camera{
	public:
		PerspectivegraphicCamera(float fov,float aspect,float zNear,float zFar);
		virtual const glm::vec3& GetPosition()const override {
			return m_Position;
		}
		virtual const glm::vec4& GetRotation()const override { return m_Rotation; }

		virtual void SetProjection(const glm::vec4& screen)override {};
		virtual void SetRotation(const glm::vec4& rotation)override;
		virtual void SetPosition(const glm::vec3& position) override{};

		virtual const glm::mat4& GetViewMatrix() const override {
			return m_ViewMatrix
				;
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

		glm::vec3 m_Position;
		glm::vec4 m_Rotation;

		};
}
	

