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
	enum class WriteMask
	{
		None = 0,
		R = BIT(0),
		G = BIT(1),
		B = BIT(2),
		A = BIT(3),
		RGB = R | G | B,
		RGBA = R | G | B | A
	};
	enum class DepthMask
	{
		None = 0,
		True = 1,
		False = 2
	};	
	enum class CullMode
	{
		None = 0,
		Front = 1,
		Back = 2,
		FrontAndBack = 3
	};
	enum class DepthFunc
	{
		Less = 0,
		LessEqual = 1,
		Greater = 2,
		GreaterEqual = 3,
		Equal = 4,
		NotEqual = 5,
		Always = 6,
		Never = 7
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
		void SetWriteMask(WriteMask mask) { m_WriteMask = (uint32_t)mask; }
		void SetCullMode(CullMode mode) { m_CullMode = (uint32_t)mode; }	
		void SetDepthFunc(DepthFunc func) { m_DepthFunc = (uint32_t)func; }	
		void SetDepthMask(DepthMask mask) { m_DepthMask = (uint32_t)mask; }
	public:
		static Ref<Material> Create(const Ref<Shader>& shader);
	private:

		Ref<Shader> m_Shader;

		uint32_t m_MaterialFlags;
		uint32_t m_WriteMask;
		uint32_t m_CullMode;
		uint32_t m_DepthFunc;
		uint32_t m_DepthMask;
	};

	//复用Material的shader,但是可以设置不同的Uniform，和Texture
	class MaterialInstance
	{
		friend class Material;
	public:
		static Ref<MaterialInstance> Create(const Ref<Shader>& shader) {
			return std::make_shared<MaterialInstance>(shader);
		}
		static Ref<MaterialInstance> Create(const Ref<Material>& material) {
			return std::make_shared<MaterialInstance>(material);
		}
	public:
		//替换成新版的使用方式
		//目前是先存储，等到用户真正要renderpass的时候再去设置uniform
		void SetUniformFloat(int locatiion, float value) {};
		void SetUniformFloat2(int locatiion, const glm::vec2& value) {};
		void SetUniformFloat3(int locatiion, const glm::vec3& value) {};
		void SetUniformFloat4(int locatiion, const glm::vec4& value) {};
		void SetUniformMat3(int locatiion, const glm::mat3& value) {};
		void SetUniformMat4(int locatiion, const glm::mat4& value);


		uint32_t AddTexture(const Ref<Texture>& texture);
		void AddTexture(const Ref<Texture>& texture, int slot) { m_Textures[slot]=texture; }
	
		void Bind() const { m_Material->Bind(); BindTextures(); }

		void UnBind() const { /*m_Material->UnBind();*/ }
		
		void ResetTexture() { m_TextureSlotIndex = 1;/*0 for white;*/ }

		void ResetAllTexture() { m_TextureSlotIndex = 0; }
		//暂时给天空盒用
		void ReplaceTexture(const Ref<Texture>& texture, int index);


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
		std::array<Ref<Texture>,32> m_Textures;
		int m_TextureSlotIndex = 0; 
		std::unordered_map<int, float> m_FloatUniforms;

	};

}