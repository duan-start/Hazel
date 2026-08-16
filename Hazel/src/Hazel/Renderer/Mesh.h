#pragma once
#include <glm/glm.hpp>
#include "Buffer.h"
#include "VertexArray.h"
namespace Hazel {

	class Material;
	class  Mesh
	{
	public:
		//同一个shader渲染下的最小单元，一般直接就是一个模型，没有可以省
		struct Vertex
		{
			glm::vec3 Position;
			glm::vec3 Normal;
			glm::vec3 Tangent;
			glm::vec3 Binormal;
			glm::vec2 Texcoord;
		};
		static_assert(sizeof(Vertex) == 14 * sizeof(float));
		static const int NumAttributes = 5;

		struct Index
		{
			uint32_t V1, V2, V3;
		};
		static_assert(sizeof(Index) == 3 * sizeof(uint32_t));

		~Mesh();
		static Ref<Mesh> Create(const std::string& filename);

	public:
		virtual const std::string& GetFilePath()const = 0;
		virtual const std::shared_ptr<VertexArray>& GetVertexArray() const =0;
		virtual bool IsLoaded() const = 0;


	};
}

