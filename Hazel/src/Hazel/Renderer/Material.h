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
		void BindTextures() const;
	private:
		Ref<Shader> m_Shader;
		std::vector<Ref<Texture>> m_Textures;
		uint32_t m_MaterialFlags;
	};

}