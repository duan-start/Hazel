#include "hzpch.h"
#include "OpenGLRendererAPI.h"
#include <glad/glad.h>

namespace Hazel {
	//��������
	//�����ϣ���ϵ�ģʽ�� ������Ȳ��ԣ�after fragshader��
	void OpenGLRendererAPI::Init()
	{
		HZ_PROFILE_FUNCTION(); 

		//������Ȳ��ԣ�������ڵ��ˣ���ֱ�Ӷ�����
		//Ȼ������Blend
		//�����Ϊʲô�����ڻ��ư�͸���������ʱ��Ҫ�Ȼ��ƺ��棨�����㷨��������Ϊ�˷�ֹ�ڱ�������ʵ��ȷʵ�ᱻ������
		glEnable(GL_BLEND);

		//��Ϊ���������������λ�ϵľ���������������������pixelshader��aֵ�����趨�ģ�
		//�Լ���src alpha,������1-src  alpha
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
		//���û�һ�����ɶȣ�Ĭ����Ⱦ���е�indexCount(input Count=0),Ҳ�����û�ָ����Ⱦ�ĸ���
		uint32_t count = indexCount ? indexCount:vertexArray->GetIndexBuffer()->GetCount() ;
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		//һ����Ҫ�ֶ��ͷ���
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}