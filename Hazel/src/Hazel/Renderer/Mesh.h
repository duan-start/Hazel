#pragma once
#include <glm/glm.hpp>
#include "Buffer.h"
#include "VertexArray.h"
namespace Hazel {

	class  Mesh
	{
	public:
		//同一个shader渲染下的最小单元，一般直接就是一个模型
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

		Mesh(const std::string& filename);
		~Mesh();

		inline const std::string& GetFilePath() const { return m_FilePath; }

		inline const std::shared_ptr<VertexArray>& GetVertexArray() const { return m_VertexArray; };
		
	private:
		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		std::vector<Vertex> m_Vertices;
		std::vector<Index> m_Indices;

		std::string m_FilePath;


	};
}

