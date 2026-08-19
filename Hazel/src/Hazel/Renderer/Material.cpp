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
		m_Shader->Bind();
	}

	void MaterialInstance::BindTextures() const
	{
		for (size_t i = 0; i < m_Textures.size(); i++)
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

	void MaterialInstance::BindTexturesOnly(Ref<Texture> texture, int slot) const
	{
		texture->Bind(slot);
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