#pragma once

#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/Texture.h"

#include <unordered_set>

namespace Hazel {

	enum class MaterialFlag
	{
		None = BIT(0),
		DepthTest = BIT(1),
		Blend = BIT(2)
	};

	class Material
	{
		friend class MaterialInstance;

	public:
		Material(const Ref<Shader>& shader);
		virtual ~Material();

		void Bind() const;
		uint32_t GetFlags() const { return m_MaterialFlags; }
		void SetFlag(MaterialFlag flag) { m_MaterialFlags |= (uint32_t)flag; }
	public:
		static Ref<Material> Create(const Ref<Shader>& shader);
	private:

		Ref<Shader> m_Shader;

		uint32_t m_MaterialFlags;
	};

	//复用Material的shader,但是可以设置不同的Uniform，和Texture
	class MaterialInstance
	{
		friend class Material;
	public:
		static Ref<MaterialInstance> Create(const Ref<Shader>& shader) {
			return std::make_shared<MaterialInstance>(shader);
		}

	public:
		//替换成新版的使用方式
		//目前是先存储，等到用户真正要renderpass的时候再去设置uniform
		void SetUniformFloat(int locatiion, float value);
		void SetUniformFloat2(int locatiion, const glm::vec2& value);
		void SetUniformFloat3(int locatiion, const glm::vec3& value);
		void SetUniformFloat4(int locatiion, const glm::vec4& value);
		void SetUniformMat3(int locatiion, const glm::mat3& value);
		void SetUniformMat4(int locatiion, const glm::mat4& value);


		void AddTexture(const Ref<Texture>& texture) { m_Textures.push_back(texture); }
		void Bind() const { m_Material->Bind(); BindTextures(); }


	public:
		MaterialInstance(const Ref<Material>& material)
			: m_Material(material) {
		}
		MaterialInstance(const Ref<Shader>& shader)
			: m_Material(Material::Create(shader)) {
		}
	private:
		void BindTextures() const;
	private:
		Ref<Material> m_Material;
		std::vector<Ref<Texture>> m_Textures;
		std::unordered_map<int, float> m_FloatUniforms;

	};

}