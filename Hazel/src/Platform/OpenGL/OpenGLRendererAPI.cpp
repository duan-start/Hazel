#include "hzpch.h"
#include "OpenGLRendererAPI.h"
#include <glad/glad.h>

namespace Hazel {
	void OpenGLRendererAPI::Init()
	{
		HZ_PROFILE_FUNCTION(); 
		glEnable(GL_BLEND);
		//为什么这里加上这个函数反而导致显示不正常
		//因为这个函数设置了如何混合的具体参数（最终是依赖你的pixelshader的a值进行设定的，所以）
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