#pragma once
#include <Hazel/Renderer/Mesh.h>

namespace Hazel {
	class OpenGLMesh:public Mesh
	{
	public:
		OpenGLMesh(const std::string& filename);
		virtual ~OpenGLMesh() {};

		virtual const std::string& GetFilePath()const { return m_FilePath; }
		virtual bool IsLoaded()const override { return m_VertexArray != nullptr; }
		virtual const std::shared_ptr<VertexArray>& GetVertexArray() const {return m_VertexArray;}
	private:
		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		std::vector<Vertex> m_Vertices;
		std::vector<Index> m_Indices;
		Ref<Material> m_Material;
		std::string m_FilePath;

	};


}

