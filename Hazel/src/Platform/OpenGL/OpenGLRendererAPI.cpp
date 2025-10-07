#include "hzpch.h"
#include "OpenGLRendererAPI.h"
#include <glad/glad.h>

namespace Hazel {
	//基础设置
	//允许混合，混合的模式， 允许深度测试（after fragshader）
	void OpenGLRendererAPI::Init()
	{
		HZ_PROFILE_FUNCTION(); 

		//先做深度测试（如果被遮挡了，就直接丢弃）
		//然后在做Blend
		//这就是为什么我们在绘制半透明的物体的时候要先绘制后面（画家算法），就是为了防止在被丢弃（实际确实会被丢弃）
		glEnable(GL_BLEND);

		//因为这个函数设置了如何混合的具体参数（最终是依赖你的pixelshader的a值进行设定的）
		//自己的src alpha,背景的1-src  alpha
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
	}
	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.x, color.y, color.z, color.w);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray,uint32_t indexCount)
	{
		//给用户一个自由度，默认渲染所有的indexCount(input Count=0),也允许用户指定渲染的个数
		uint32_t count = indexCount ? indexCount:vertexArray->GetIndexBuffer()->GetCount() ;
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		//一定需要手动释放吗
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}