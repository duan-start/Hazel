#include "hzpch.h"
#include "OpenGLTexture2D.h"
#include "stb_image.h"

#include <glad/glad.h>
#include <filesystem>

namespace Hazel {

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
	{
		HZ_PROFILE_FUNCTION();
		//����ֻ�����úó�ʼ��gpu�ģ�����û��data
		m_Height = height;
		m_Width = width;

		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;

		// ����OpenGL���� ,�����������gpu��ν���ʹ洢�������
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

		//����gpu ��������ݸ�ʽ��internal��
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height); // ʹ���µı�����  

		//��learnopengl �����glTexParameteriҪ���Ӹ߼��㣬����ֱ�����ö�Ӧid��texutr,������Ҫ�ֶ���textre,Ȼ��������
		// �����������,���ù������ͣ��Ŵ���˺���С���ˣ��������mag��,�����С��min��
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//���ó����߽�Ļ������
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path):
		m_path(path)
	{
		HZ_PROFILE_FUNCTION();
		int height, width, channels;
		//����Ĭ��ͼ��������opengl��y���෴�ģ���uv���꣩
		//�������ֱ�ӷ�תstbi�����ͼ�����Ϣ
		stbi_set_flip_vertically_on_load(1);

		stbi_uc* data = nullptr;
		{
			HZ_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std:string&)");
			//std::cout << "Working dir: " << std::filesystem::current_path() << std::endl;

			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}
		if(data) m_IsLoaded = true;
		HZ_CORE_ASSERT(data, "Failed To Load Image");
		HZ_CORE_ASSERT(width > 0 && height > 0, "Loaded image has zero dimensions.");
		
		m_Height = height;
		m_Width = width;

		//Ϊ�˼��ݵ�һ��Hack���ֶ�
		
		if (channels == 1)
			m_InternalFormat = GL_RED;
		else if (channels == 3) {
			m_InternalFormat = GL_RGB8;
			m_DataFormat = GL_RGB;
		}	
		else if (channels == 4)
		{
			m_InternalFormat = GL_RGBA8;
			m_DataFormat = GL_RGBA;
		}
		HZ_CORE_ASSERT(m_InternalFormat & m_DataFormat, "Format not supported");

		// ����OpenGL���� ,�����������gpu��ν���ʹ洢�������
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

		//����gpu ��������ݸ�ʽ��internal��
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height); // ʹ���µı�����  

		//��learnopengl �����glTexParameteriҪ���Ӹ߼��㣬����ֱ�����ö�Ӧid��texutr,������Ҫ�ֶ���textre,Ȼ��������
		// �����������,���ù������ͣ��Ŵ���˺���С���ˣ��������mag��,�����С��min��
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//���ó����߽�Ļ������
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// ��ʽ�ϴ��������� ��cpu->gpu��
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		HZ_PROFILE_FUNCTION();
		glDeleteTextures(1, &m_RendererID);
	}

    bool OpenGLTexture2D::operator==(const Texture& other) const  
    {  
       return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;  
    }
	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		HZ_PROFILE_FUNCTION();
		//������һ����׳�ԵĲ���
		//ȷ��size ���������
		uint32_t bpp = 4;
		HZ_CORE_ASSERT(size==m_Width*m_Height*bpp,"Wrong Texture Size");
		// ��ʽ�ϴ��������� ��cpu->gpu��
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);

	}
	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		HZ_PROFILE_FUNCTION();
		glBindTextureUnit(slot,m_RendererID);
	}
}