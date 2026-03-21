#include "hzpch.h"
#include "Mesh.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include <glad/glad.h>

namespace Hazel {

	namespace {
		const unsigned int ImportFlags =
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_SortByPType |
			aiProcess_PreTransformVertices |
			aiProcess_GenNormals |
			aiProcess_GenUVCoords |
			aiProcess_OptimizeMeshes |
			aiProcess_Debone |
			aiProcess_ValidateDataStructure;
	}

	struct LogStream : public Assimp::LogStream
	{
		static void Initialize()
		{
			if (Assimp::DefaultLogger::isNullLogger())
			{
				Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
				Assimp::DefaultLogger::get()->attachStream(new LogStream, Assimp::Logger::Err | Assimp::Logger::Warn);
			}
		}

		void write(const char* message) override
		{
			HZ_CORE_ERROR("Assimp error: {0}", message);
		}
	};

	Mesh::Mesh(const std::string& filename)
		: m_FilePath(filename)
	{
		m_VertexArray=VertexArray::Create();

		LogStream::Initialize();

		HZ_CORE_INFO("Loading mesh: {0}", filename.c_str());

		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(filename, ImportFlags);
		if (!scene || !scene->HasMeshes())
			HZ_CORE_ERROR("Failed to load mesh file: {0}", filename);

		aiMesh* mesh = scene->mMeshes[0];

		HZ_CORE_ASSERT(mesh->HasPositions(), "Meshes require positions.");
		HZ_CORE_ASSERT(mesh->HasNormals(), "Meshes require normals.");

		m_Vertices.reserve(mesh->mNumVertices);

		// Extract vertices from model
		for (size_t i = 0; i < m_Vertices.capacity(); i++)
		{
			Vertex vertex;
			vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
			vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

			if (mesh->HasTangentsAndBitangents())
			{
				vertex.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
				vertex.Binormal = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
			}

			if (mesh->HasTextureCoords(0))
				vertex.Texcoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
			m_Vertices.push_back(vertex);
		}

		m_VertexBuffer=VertexBuffer::Create(m_Vertices.size() * sizeof(Vertex));
		m_VertexBuffer->SetData(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));

		//SetLayout
				//设置顶点属性
		{
			Hazel::BufferLayout layout = {
				//QuadVertex Set
				{ ShaderDataType::Float3, "a_Pos"},
				{ ShaderDataType::Float3, "a_Nor" },
				{ ShaderDataType::Float3, "Tan" },
				{ ShaderDataType::Float3,  "Bin"},
				//a_Fra代表的是平铺因子
				{ ShaderDataType::Float2,  "a_Tex"},
			};
			//vbo设置好空间和属性
			m_VertexBuffer->SetLayout(layout);
		}
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);


		// Extract indices from model
		m_Indices.reserve(mesh->mNumFaces);
		for (size_t i = 0; i < m_Indices.capacity(); i++)
		{
			HZ_CORE_ASSERT(mesh->mFaces[i].mNumIndices == 3, "Must have 3 indices.");
			m_Indices.push_back({ mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2] });
		}

		m_IndexBuffer = IndexBuffer::Create(m_Indices.size() * sizeof(Index));
		m_IndexBuffer->SetData(m_Indices.data(), m_Indices.size() * sizeof(Index));


		m_VertexArray->SetIndexBuffer(m_IndexBuffer);


		{
			HZ_CORE_WARN("Mesh Loaded: {0}", filename);
			HZ_CORE_WARN("  Vertices: {0} (Size: {1} bytes)", m_Vertices.size(), m_Vertices.size() * sizeof(Vertex));
			HZ_CORE_WARN("  Indices:  {0}", m_Indices.size() * 3);

			// 看看第一个顶点的坐标，确认不是 (0,0,0) 或者极其离谱的数
			if (!m_Vertices.empty()) {
				HZ_CORE_TRACE("  First Vertex Pos: {0}, {1}, {2}", m_Vertices[0].Position.x, m_Vertices[0].Position.y, m_Vertices[0].Position.z);
			}
		}
	}

	Mesh::~Mesh()
	{
	}

}