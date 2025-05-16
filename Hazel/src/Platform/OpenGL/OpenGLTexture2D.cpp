#include "hzpch.h"
#include "OpenGLTexture2D.h"
#include "stb_image.h"

#include <glad/glad.h>

namespace Hazel {
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path):
		m_path(path)
	{
		int height, width, channels;
		//����Ĭ��ͼ��������opengl��y���෴�ģ���uv���꣩
		//�������ֱ�ӷ�תstbi�����ͼ�����Ϣ
		stbi_set_flip_vertically_on_load(1);

		stbi_uc* data = stbi_load(path.c_str(), (int*)&width, (int*)&height, (int*)&channels, 0);
		HZ_CORE_ASSERT(data, "Failed To Load Image");
		HZ_CORE_ASSERT(width > 0 && height > 0, "Loaded image has zero dimensions.");

		m_Height = height;
		m_Width = width;

		//Ϊ�˼��ݵ�һ��Hack���ֶ�
		GLenum internalFormat =0,dataFormat=0;
		if (channels == 1)
			internalFormat = GL_RED;
		else if (channels == 3) {
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}	
		else if (channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		HZ_CORE_ASSERT(internalFormat & dataFormat, "Format not supported");

		// ����OpenGL���� ,�����������gpu��ν���ʹ洢�������
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height); // ʹ���µı�����  

		//��learnopengl �����glTexParameteriҪ���Ӹ߼��㣬����ֱ�����ö�Ӧid��texutr,������Ҫ�ֶ���textre,Ȼ��������
		// �����������,���ù������ͣ��Ŵ���˺���С���ˣ��������mag��,�����С��min��
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//���ó����߽�Ļ������
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// �ϴ��������� �����������ʼ�ϴ���������ʲô��ʽ
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}
	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}
	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot,m_RendererID);
	}
}