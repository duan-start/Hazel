#pragma once
#include "Hazel/Renderer/Framebuffer.h"
#include <glad/glad.h>

namespace Hazel {

	class OpenGLFramebuffer:public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		  virtual ~OpenGLFramebuffer();

		  void Invalidate();

		  virtual void Bind() override;
		  virtual void Unbind() override;
		  virtual void Resize(uint32_t width, uint32_t height) override;
		  virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;
		  virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;
		  virtual uint32_t GetColorAttachmentRendererID(uint32_t index) const override { return m_ColorAttachments[index]; }
		  virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }
	private:
		uint32_t m_RendererID=0;
		//uint32_t m_ColorAttachment=0, m_DepthAttachment=0;
		//参数配置
		FramebufferSpecification m_Specification;

		//纹理附件（保存的是每张颜色纹理附件的格式）
		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		//深度纹理的格式
		FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;

		//实际的handle
		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};


}

