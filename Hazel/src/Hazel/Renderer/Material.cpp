#include "hzpch.h"
#include "Material.h"

//todo remove

#include "Hazel/Renderer/UniformBuffer.h"
#include "glad/glad.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
namespace Hazel {

	//////////////////////////////////////////////////////////////////////////////////
	// Material
	//////////////////////////////////////////////////////////////////////////////////

	Ref<Material> Material::Create(const Ref<Shader>& shader)
	{
		return std::make_shared<Material>(shader);
	}

	Material::Material(const Ref<Shader>& shader)
		: m_Shader(shader)
	{
		m_MaterialFlags |= (uint32_t)MaterialFlag::DepthTest;
		m_MaterialFlags |= (uint32_t)MaterialFlag::Blend;
	}

	Material::~Material()
	{
	}

	void Material::Bind() const
	{
		//设置材质的渲染状态
		//设置写入颜色的mask
		//glColorMask((m_WriteMask & (uint32_t)WriteMask::R) != 0, (m_WriteMask & (uint32_t)WriteMask::G) != 0, (m_WriteMask & (uint32_t)WriteMask::B) != 0, (m_WriteMask & (uint32_t)WriteMask::A) != 0);
		
		switch (m_DepthFunc) {
		case (uint32_t)DepthFunc::Less:glDepthFunc(GL_LESS);break;
		case (uint32_t)DepthFunc::Equal:glDepthFunc(GL_EQUAL);break;
		case (uint32_t)DepthFunc::Greater:glDepthFunc(GL_GREATER);break;
		default:
			// 默认回退，最常用小于
			glDepthFunc(GL_LESS);
			break;
		}
		switch(m_DepthMask) {
		case (uint32_t)DepthMask::None: glDepthMask(GL_FALSE); break;
		case (uint32_t)DepthMask::True: glDepthMask(GL_TRUE); break;
		case (uint32_t)DepthMask::False: glDepthMask(GL_FALSE); break;
		default:glDepthMask(GL_TRUE);break;
		}

		m_Shader->Bind();
	}

	void MaterialInstance::BindTextures() const
	{
		for (size_t i = 0; i <m_TextureSlotIndex; i++)
		{
			auto& texture = m_Textures[i];
			if (texture)
				texture->Bind(i);
		}
	}


	void MaterialInstance::SetUniformMat4(int locatiion, const glm::mat4& value)
	{
		//还是得用Uniform的方式来设置shader的uniform变量
		//因为vk根本不支持用location设置Uniform变量
		Ref<UniformBuffer> uniformBuffer = UniformBuffer::Create(sizeof(glm::mat4), locatiion);
		uniformBuffer->SetData(&value, sizeof(glm::mat4),0);
	}

	uint32_t MaterialInstance::AddTexture(const Ref<Texture>& texture)
	{
		uint32_t textureIndex = 0;
		for (uint32_t i = 1; i < m_TextureSlotIndex; i++) {
			if (*m_Textures[i].get() == *texture.get()) {
				textureIndex = i;
				break;
			}
		}
		if (textureIndex == 0) {
			textureIndex = m_TextureSlotIndex;
			m_Textures[m_TextureSlotIndex++] = texture;
		}
		return textureIndex;
	}


	void MaterialInstance::ReplaceTexture(const Ref<Texture>& texture, int index)
	{
		if(index < 0 || index >= m_Textures.size())
		{
			HZ_CORE_ERROR("ReplaceTexture: Index out of bounds");
			return;
		}
		m_Textures[index] = texture;
	}

}