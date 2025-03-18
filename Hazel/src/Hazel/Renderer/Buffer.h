#pragma once

namespace Hazel {
	enum class ShaderDataType {
		None=0,
		Float,Float2,Float3,Float4,
		Int,Int2,Int3,Int4,
		Mat3,Mat4,
		Bool,
	};
	static uint32_t GetSize(ShaderDataType& type) {
		switch (type) {
		case ShaderDataType::None:				return 4 * 0;
		case ShaderDataType::Float:				return 4 * 1;
		case ShaderDataType::Float2:			return 4 * 2;
		case ShaderDataType::Float3:			return 4 * 3;
		case ShaderDataType::Float4:			return 4 * 4;
		case ShaderDataType::Int:				return 4 * 1;
		case ShaderDataType::Int2:				return 4 * 2;
		case ShaderDataType::Int3:				return 4 * 3;
		case ShaderDataType::Int4:				return 4 * 4;
		case ShaderDataType::Mat3:				return 4 * 4 * 3;
		case ShaderDataType::Mat4:				return 4 * 4 * 4;
		case ShaderDataType::Bool:				return  1;
		}
		HZ_CORE_ASSERT(false, "Unknowen ShaderDataType");
		return 0;
	}


	struct BufferElements {
		BufferElements() {};

		//这种struct 里面的成员和枚举的都是首字母大写
		std::string Name;
		uint32_t Offset;
		uint32_t Size;
		ShaderDataType Type;
		bool Normalized;
		BufferElements(ShaderDataType type ,const std::string& name,bool normalized=false):Name(name), Offset(0), Size(GetSize(type)), Type(type),Normalized(normalized)
		{

		}

		uint32_t GetComponentCount()const {
			switch (Type) {
			case ShaderDataType::Float :	return 1;
			case ShaderDataType::Float2:	return 2;
			case ShaderDataType::Float3:	return 3;
			case ShaderDataType::Float4:	return 4;
			case ShaderDataType::Mat3:		return 3*3;
			case ShaderDataType::Mat4:		return 4*4;
			case ShaderDataType::Int:		return 1;
			case ShaderDataType::Int2:		return 2;
			case ShaderDataType::Int3:		return 3;
			case ShaderDataType::Int4:		return 4;
			case ShaderDataType::Bool:		return 1;
			}
			HZ_CORE_ASSERT(false, "Unknowen ShaderDataType");
			return 0;
		}

	};

	class BufferLayout {
	public:
		BufferLayout() {};
		//这个好像是c++11开头的那个新特性
		BufferLayout(const std::initializer_list<BufferElements>& BufferElements) :m_BufferElements(BufferElements) {
			CalculateOffsetAndStride();
		};
		
		inline uint32_t GetStride()const { return m_Stride; }

		inline const std::vector<BufferElements>& Get() { return m_BufferElements; }
		std::vector<BufferElements>::iterator begin() {
			return m_BufferElements
				.begin();
		}
		std::vector<BufferElements>::iterator end() {
			return m_BufferElements
				.end();
		}

		std::vector<BufferElements>::const_iterator begin() const{
		  return m_BufferElements
				.begin();
		}
		std::vector<BufferElements>::const_iterator end() const {
			return m_BufferElements
				.end();
		}
		
	private:
		void CalculateOffsetAndStride() {
			uint32_t offset = 0;
			m_Stride = 0;
			//计算偏移量和字符
			for (auto& element : m_BufferElements) {
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}
	private:
		std::vector<BufferElements> m_BufferElements;
		uint32_t m_Stride = 0;
	};



	class VerTexBuffer
	{
	public:
		virtual ~VerTexBuffer();
		virtual	void Bind()const = 0;
		virtual void UBind()const = 0;
		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;
		static VerTexBuffer* Creat(float* vertices,uint32_t size);
	};
	
	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer();
		virtual	void Bind()const = 0;
		virtual void UBind()const = 0;
		static IndexBuffer* Creat(uint32_t* indices, uint32_t count);
	};

}


