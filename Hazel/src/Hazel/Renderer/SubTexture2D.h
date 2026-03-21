#pragma once

#include <glm/glm.hpp> 
#include "Hazel/Renderer/Texture.h"
namespace Hazel {

	//纹理图集，实现纹理合并，减少slot的压力(core)
	class SubTexture2D
	{
	public:
		//如果知道包围盒的大小的话
		SubTexture2D(const Ref<Texture2D>& texture2D, const glm::vec2& min, const glm::vec2& max);

		const Ref<Texture2D>& GetTexture2D() const {
			return m_Texture2D;
		}
		const glm::vec2* GetTexCoords()const {
			return m_TexCoords;
		}
		//更简单的创建方法
		static Ref<SubTexture2D> CreateFromCoords(const Ref<Texture2D>& texture2D, const glm::vec2& coords, const glm::vec2& spriteSize);
	private:
		Ref<Texture2D> m_Texture2D;
		glm::vec2 m_TexCoords[4];
	};
}

