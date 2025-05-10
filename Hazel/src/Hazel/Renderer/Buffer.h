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
		//因为设置一种顶点属性，我们需要
		//1. index：顶点属性的索引，通常对应于顶点着色器中的 layout(location = index)。
		//2. size：每个顶点属性的组件数量，可以是1、2、3或4。
		//3. type：数据类型，如 GL_FLOAT、GL_INT 等。
		//4. normalized：是否将整数数据归一化到[0, 1] 或[-1, 1] 范围。
		//5. stride：每个顶点之间的字节偏移量。如果所有顶点属性紧密排列，可以设置为0。
		//5. pointer：数据在缓冲区中的偏移量，通常是一个字节偏移量。
		//然后由于index没有必要，Size大小实际是给下一个elements用的
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

		inline const std::vector<BufferElements>& Get() const{ return m_BufferElements; }
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
				//偏移量就是直接记录到上次的对应的(所以单独记录了offset和element.offset)，
				// 至于这个stride就是必须得加上所有的Size()
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
		static Ref<VerTexBuffer> Creat(float* vertices,uint32_t size);
	};
	
	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer();
		virtual	void Bind()const = 0;
		virtual void UBind()const = 0;
		virtual uint32_t GetCount() const = 0;
		static IndexBuffer* Creat(uint32_t* indices, uint32_t count);
	};

}


