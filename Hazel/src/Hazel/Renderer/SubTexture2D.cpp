#include "hzpch.h"
#include "SubTexture2D.h"

namespace Hazel {
	SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture2D, const glm::vec2& min, const glm::vec2& max):m_Texture2D(texture2D)
	{
		m_TexCoords[0] = { min.x ,min.y};
		m_TexCoords[1] = { max.x ,min.y };
		m_TexCoords[2] = { max.x ,max.y };
		m_TexCoords[3] = { min.x ,max.y };

	}
	Ref<SubTexture2D> SubTexture2D::CreateFromCoords(const Ref<Texture2D>& texture2D, const glm::vec2& coords, const glm::vec2& ceilSize)
	{
		//coords表示index（表示第几行，第几列）
		//ceilSize表示宽和高
		//获得对应的Bounding box;
		glm::vec2 min = { coords.x * ceilSize.x/texture2D->GetWidth() ,coords.y * ceilSize.y/ texture2D->GetHeight() };
		glm::vec2 max = { (coords.x + 1) * ceilSize.x/ texture2D->GetWidth()  ,(coords.y + 1) * ceilSize.y/ texture2D->GetHeight() };

		Ref<SubTexture2D> result=CreateRef<SubTexture2D>(texture2D,min,max);
		return result;
	}
}