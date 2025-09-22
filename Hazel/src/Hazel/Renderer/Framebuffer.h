#pragma once
#include "Hazel/Core/Core.h"

namespace Hazel {

	//MRTS 多重渲染目标，想要在渲染的过程中渲染出多张图,使用不同的shader进行渲染，然后用RGBA的数据存储EntityID(不是实际的，只是用来选中的一个顺序代号)，
	//再根据鼠标点击的采样，得到真实选中的实体，再imgui中选中并直接进行调整
	
	//设置更加灵活多变的接口

	//framebuffer 渲染是使用对应的shader如何进行输出的每个pixel的值
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

	//作为存储渲染纹理的控制类
	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format)
			: TextureFormat(format) {
		}

		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
		// TODO: filtering/wrap
	};

	//vector<控制类>，可以同时添加多个渲染的纹理
	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
			: Attachments(attachments) {
		}

		std::vector<FramebufferTextureSpecification> Attachments;
	};



	struct FramebufferSpecification {
		uint32_t Width, Height;
		FramebufferAttachmentSpecification  Attachments;
		uint32_t Samples = 1;
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
		  virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;

		  virtual const FramebufferSpecification& GetSpecification() const = 0;

		  static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
	};

}

