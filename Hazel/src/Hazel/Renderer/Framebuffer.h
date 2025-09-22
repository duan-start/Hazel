#pragma once
#include "Hazel/Core/Core.h"

namespace Hazel {

	//MRTS ������ȾĿ�꣬��Ҫ����Ⱦ�Ĺ�������Ⱦ������ͼ,ʹ�ò�ͬ��shader������Ⱦ��Ȼ����RGBA�����ݴ洢EntityID(����ʵ�ʵģ�ֻ������ѡ�е�һ��˳�����)��
	//�ٸ���������Ĳ������õ���ʵѡ�е�ʵ�壬��imgui��ѡ�в�ֱ�ӽ��е���
	
	//���ø��������Ľӿ�

	//framebuffer ��Ⱦ��ʹ�ö�Ӧ��shader��ν��������ÿ��pixel��ֵ
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

	//��Ϊ�洢��Ⱦ����Ŀ�����
	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format)
			: TextureFormat(format) {
		}

		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
		// TODO: filtering/wrap
	};

	//vector<������>������ͬʱ��Ӷ����Ⱦ������
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

