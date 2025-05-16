#include "hzpch.h"
#include "OpenGLTexture2D.h"
#include "stb_image.h"

#include <glad/glad.h>

namespace Hazel {
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path):
		m_path(path)
	{
		int height, width, channels;
		//由于默认图像的坐标和opengl是y轴相反的，（uv坐标）
		//所以这边直接反转stbi处理的图像的信息
		stbi_set_flip_vertically_on_load(1);

		stbi_uc* data = stbi_load(path.c_str(), (int*)&width, (int*)&height, (int*)&channels, 0);
		HZ_CORE_ASSERT(data, "Failed To Load Image");
		HZ_CORE_ASSERT(width > 0 && height > 0, "Loaded image has zero dimensions.");

		m_Height = height;
		m_Width = width;

		//为了兼容的一种Hack的手段
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

		// 创建OpenGL纹理 ,这个决定的是gpu如何解读和存储这个纹理
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height); // 使用新的变量名  

		//比learnopengl 里面的glTexParameteri要更加高级点，可以直接设置对应id的texutr,而不需要手动绑定textre,然后再设置
		// 设置纹理参数,设置过滤类型（放大过滤和缩小过滤）纹理过大（mag）,纹理过小（min）
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//设置超出边界的环绕情况
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// 上传纹理数据 ，这个决定初始上传的纹理是什么格式
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