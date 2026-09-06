#include "hzpch.h"
#include "OpenGLTextureCube.h"
#include "OpenGLRendererAPI.h"
#include "stb_image.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <cmath>

namespace Hazel {

	static GLenum HazelToOpenGLTextureFormat(TextureFormat format)
	{
		switch (format)
		{
		case Hazel::TextureFormat::RGB:      return GL_RGB;
		case Hazel::TextureFormat::RGBA:     return GL_RGBA;
		case Hazel::TextureFormat::RGB16F:   return GL_RGB16F;
		case Hazel::TextureFormat::RGBA16F:  return GL_RGBA16F;
		}
		return 0;
	}

	namespace {
		// 4x3 十字布局：从大图里拷贝一个面
		template<typename T>
		void ExtractCrossFace(const T* src, T* dst, int srcW, int srcH, int faceW, int faceH, size_t xOff, size_t yOff)
		{
			for (size_t y = 0; y < (size_t)faceH; y++)
				for (size_t x = 0; x < (size_t)faceW; x++)
				{
					size_t s = ((yOff + y) * (size_t)srcW + (xOff + x)) * 3;
					size_t d = (y * (size_t)faceW + x) * 3;
					dst[d + 0] = src[s + 0];
					dst[d + 1] = src[s + 1];
					dst[d + 2] = src[s + 2];
				}
		}

		// 等距柱状图（equirectangular, 2:1）按方向采样
		template<typename T>
		void SampleEquirect(const T* src, int srcW, int srcH, const glm::vec3& dir, T* out)
		{
			float u = std::atan2(dir.z, dir.x) / (2.0f * 3.1415926535897932f) + 0.5f;
			// Radiance HDR 的 "-Y" 布局：文件第一行 = 图像顶部 = 天空（天顶）。
			// stb 不翻转时缓冲第 0 行 = 文件第一行，对应纹理 v=0。
			// 所以天顶（+Y）必须映射到 v=0，即 v = 0.5 - asin(y)/PI；
			// 之前用 v = 0.5 + asin(y)/PI 会把天空渲染到地面方向（上下颠倒）。
			float v = 0.5f - std::asin(glm::clamp(dir.y, -1.0f, 1.0f)) / 3.1415926535897932f;
			int x = (int)(u * srcW) % srcW;
			int y = (int)(v * srcH);
			if (y < 0) y = 0;
			if (y >= srcH) y = srcH - 1;
			size_t s = ((size_t)y * srcW + (size_t)x) * 3;
			out[0] = src[s + 0];
			out[1] = src[s + 1];
			out[2] = src[s + 2];
		}

		// 把一个立方体面（0..5 = +X,-X,+Y,-Y,+Z,-Z）投影到等距柱状图上
		template<typename T>
		void ProjectEquirectFace(const T* src, T* dst, int srcW, int srcH, int faceSize, int faceId)
		{
			for (int y = 0; y < faceSize; y++)
			{
				float t = (y + 0.5f) / faceSize;
				for (int x = 0; x < faceSize; x++)
				{
					float s = (x + 0.5f) / faceSize;
					glm::vec3 dir;
					switch (faceId)
					{
					case 0: dir = glm::normalize(glm::vec3( 1.0f, -(2.0f*t - 1.0f), -(2.0f*s - 1.0f))); break;
					case 1: dir = glm::normalize(glm::vec3(-1.0f, -(2.0f*t - 1.0f),  (2.0f*s - 1.0f))); break;
					case 2: dir = glm::normalize(glm::vec3( (2.0f*s - 1.0f),  1.0f,  (2.0f*t - 1.0f))); break;
					case 3: dir = glm::normalize(glm::vec3( (2.0f*s - 1.0f), -1.0f, -(2.0f*t - 1.0f))); break;
					case 4: dir = glm::normalize(glm::vec3( (2.0f*s - 1.0f), -(2.0f*t - 1.0f),  1.0f)); break;
					default:dir = glm::normalize(glm::vec3(-(2.0f*s - 1.0f), -(2.0f*t - 1.0f), -1.0f)); break;
					}
					SampleEquirect(src, srcW, srcH, dir, dst + ((size_t)y * faceSize + x) * 3);
				}
			}
		}
	}

	OpenGLTextureCube::OpenGLTextureCube(const std::string& path)
		: m_FilePath(path)
	{
		int width = 0, height = 0, channels = 0;
		stbi_set_flip_vertically_on_load(false);
		//用stbi_is_hdr来判断是否是HDR图像，stbi_loadf加载HDR图像，stbi_load加载普通图像
		m_IsHDR = stbi_is_hdr(path.c_str()) == 1;
		m_ImageData = nullptr;
		m_ImageDataFloat = nullptr;

		if (m_IsHDR)
			m_ImageDataFloat = stbi_loadf(path.c_str(), &width, &height, &channels, STBI_rgb);
		else
			m_ImageData = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb);

		HZ_CORE_ASSERT((m_ImageData || m_ImageDataFloat) && width > 0 && height > 0, "Failed to load texture cube image!");

		m_Width = width;
		m_Height = height;
		m_Format = m_IsHDR ? TextureFormat::RGB16F : TextureFormat::RGB;

		// 布局自适应：2:1 等距柱状图投影成 6 面；4:3 按 4x3 十字提取
		const bool equirect = (m_Width == m_Height * 2);
		unsigned int faceSize = equirect ? (m_Height / 2) : (m_Width / 4);
		HZ_CORE_INFO("TextureCube: {0} ({1}x{2}) hdr={3} equirect={4} face={5}x{5}", m_FilePath, m_Width, m_Height, m_IsHDR, equirect, faceSize);

		std::array<unsigned char*, 6> facesByte{};
		std::array<float*, 6> facesFloat{};
		for (size_t i = 0; i < 6; i++)
		{
			if (m_IsHDR)
				facesFloat[i] = new float[faceSize * faceSize * 3];
			else
				facesByte[i] = new unsigned char[faceSize * faceSize * 3];
		}

		if (equirect)
		{
			for (int face = 0; face < 6; face++)
			{
				if (m_IsHDR)
					ProjectEquirectFace(m_ImageDataFloat, facesFloat[face], m_Width, m_Height, faceSize, face);
				else
					ProjectEquirectFace(m_ImageData, facesByte[face], m_Width, m_Height, faceSize, face);
			}
		}
		else
		{
			// 4x3 十字：中间一行 4 个侧面，第 1 列上下为 +Y/-Y
			int faceIndex = 0;
			for (size_t i = 0; i < 4; i++)
			{
				if (m_IsHDR)
					ExtractCrossFace(m_ImageDataFloat, facesFloat[faceIndex], m_Width, m_Height, faceSize, faceSize, i * faceSize, faceSize);
				else
					ExtractCrossFace(m_ImageData, facesByte[faceIndex], m_Width, m_Height, faceSize, faceSize, i * faceSize, faceSize);
				faceIndex++;
			}
			for (size_t i = 0; i < 3; i++)
			{
				if (i == 1)
					continue;
				if (m_IsHDR)
					ExtractCrossFace(m_ImageDataFloat, facesFloat[faceIndex], m_Width, m_Height, faceSize, faceSize, faceSize, i * faceSize);
				else
					ExtractCrossFace(m_ImageData, facesByte[faceIndex], m_Width, m_Height, faceSize, faceSize, faceSize, i * faceSize);
				faceIndex++;
			}
		}

		//SetParamter
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		auto format = HazelToOpenGLTextureFormat(m_Format);
		GLenum dataType = m_IsHDR ? GL_FLOAT : GL_UNSIGNED_BYTE;

		auto facePtr = [&](int idx) -> const void*
		{
			return m_IsHDR ? (const void*)facesFloat[idx] : (const void*)facesByte[idx];
		};

		// 等距投影生成的面顺序是 +X,-X,+Y,-Y,+Z,-Z（faces[0..5]）；
		// 十字提取的面顺序是 中间行[左,中1,中2,右]=[-X,+Z,+X,-Z]，上/+Y、下/-Y。
		// 两者上传索引不同，必须分开映射，否则六个面会互相错位。
		if (equirect)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, faceSize, faceSize, 0, GL_RGB, dataType, facePtr(0));
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, faceSize, faceSize, 0, GL_RGB, dataType, facePtr(1));
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, faceSize, faceSize, 0, GL_RGB, dataType, facePtr(2));
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, faceSize, faceSize, 0, GL_RGB, dataType, facePtr(3));
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, faceSize, faceSize, 0, GL_RGB, dataType, facePtr(4));
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, faceSize, faceSize, 0, GL_RGB, dataType, facePtr(5));
		}
		else
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, faceSize, faceSize, 0, GL_RGB, dataType, facePtr(2));
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, faceSize, faceSize, 0, GL_RGB, dataType, facePtr(0));
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, faceSize, faceSize, 0, GL_RGB, dataType, facePtr(4));
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, faceSize, faceSize, 0, GL_RGB, dataType, facePtr(5));
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, faceSize, faceSize, 0, GL_RGB, dataType, facePtr(1));
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, faceSize, faceSize, 0, GL_RGB, dataType, facePtr(3));
		}

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		for (size_t i = 0; i < 6; i++)
		{
			if (m_IsHDR)
				delete[] facesFloat[i];
			else
				delete[] facesByte[i];
		}

		if (m_IsHDR)
			stbi_image_free(m_ImageDataFloat);
		else
			stbi_image_free(m_ImageData);
	}

	OpenGLTextureCube::~OpenGLTextureCube()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTextureCube::Bind(unsigned int slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}
}
