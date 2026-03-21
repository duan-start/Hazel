#pragma once
#include "Hazel/Core/Core.h"

namespace Hazel {

	//MRTS 多重渲染目标，离屏渲染到多张纹理图上
	enum class FramebufferTextureFormat
	{
		None = 0,
		// Color
		RGBA8,
		RED_INTEGER,
		// Depth/stencil
		DEPTH24STENCIL8,
		// Defaults
		Depth = DEPTH24STENCIL8
	};

	//单个纹理附件
	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format)
			: TextureFormat(format) {
		}
		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
		// TODO: filtering/wrap
	};

	//多个纹理附件
	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
			: Attachments(attachments) {
		}
		std::vector<FramebufferTextureSpecification> Attachments;
	};

	//参数作为结构体封装
	struct FramebufferSpecification {
		uint32_t Width, Height;
		FramebufferAttachmentSpecification  Attachments;
		//多重采样-》纹理缓冲区大小
		uint32_t Samples = 1;
		//交换链表示屏幕
		bool SwapChainTarget = false;
	};

	class Framebuffer {
	public:
		virtual ~Framebuffer() = default;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t indexID=0) const = 0;

		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;
		//手动清理某个附件
		virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;
		virtual const FramebufferSpecification& GetSpecification() const = 0;

		//简单工厂
		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
	};

}

