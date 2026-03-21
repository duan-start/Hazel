#pragma once

namespace Hazel {
	enum class ShaderDataType {
		None=0,
		Float,Float2,Float3,Float4,
		Int,Int2,Int3,Int4,
		Mat3,Mat4,
		Bool,
	};
	//工具函数，全局，计算字节大小
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

	//每个顶点的属性状态
	struct BufferElements {
		//设置顶点属性所需要的参数：
		//1. index：顶点属性的索引，对应于顶点着色器中的 layout(location = index)。nope
		//2. size：每个属性对应的数量（也就是count,如vec3->3）
		//3. type：数据类型，如 GL_FLOAT、GL_INT (一般都是GL_FLOAT)
		//4. normalized：是否将整数数据归一化到[0, 1] 或[-1, 1] 范围。
		//5. stride：每个顶点之间的字节偏移量。(理解为到下一个顶点的同样的数据中的字节量)
		//6. offset：(const void *)字节偏移量,表示该属性的起始偏移量 
		BufferElements() {};

		//数据成员首字母大写
		std::string Name;

		uint32_t Offset;
		uint32_t Size;
		ShaderDataType Type;
		bool Normalized;
		//分离情况--看看哪些参数是需要前面参数的信息才能求出来的
		BufferElements(ShaderDataType type ,const std::string& name,bool normalized=false):Name(name), Offset(0), Size(GetSize(type)), Type(type),Normalized(normalized)
		{
		}
		//获取这个类型代表多少个分量，设置size的时候用
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

	//缓冲区的格式布局，vbo和vao通信的基础，vector<attribute>
	class BufferLayout {
	public:
		BufferLayout() {};

		//Initializer List进行初始化，能够可变长
		BufferLayout(const std::initializer_list<BufferElements>& BufferElements) :m_BufferElements(BufferElements) {
			CalculateOffsetAndStride();
		};
		
		inline uint32_t GetStride()const { return m_Stride; }

		inline const std::vector<BufferElements>& Get() const{ return m_BufferElements; }

		std::vector<BufferElements>::iterator begin() {
			return m_BufferElements.begin();
		}
		std::vector<BufferElements>::iterator end() {
			return m_BufferElements.end();
		}

		std::vector<BufferElements>::const_iterator begin() const{
		  return m_BufferElements.begin();
		}
		std::vector<BufferElements>::const_iterator end() const {
			return m_BufferElements.end();
		}
		
	private:
		void CalculateOffsetAndStride() {
			uint32_t offset = 0;
			m_Stride = 0;
			//计算偏移量和字符
			for (auto& element : m_BufferElements) {
				//该元素的的偏移量=sizeof(former)
				element.Offset = offset;
				offset += element.Size;
				//该bufferlayout的步长=sizeof(all)
				m_Stride += element.Size;
			}
		}
	private:
		std::vector<BufferElements> m_BufferElements;
		//m_Stride是统一的
		uint32_t m_Stride = 0;
	};


	//vbo buffer 
	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer();
		virtual	void Bind()const = 0;
		virtual void UBind()const = 0;
		//填充数据
		virtual void SetData(const void* data, uint32_t size) = 0;
		virtual const BufferLayout& GetLayout() const = 0;
		
		//设置布局，实现和vao之间的通信（vao layout设置）---core
		virtual void SetLayout(const BufferLayout& layout) = 0;

		static Ref<VertexBuffer> Create(float* vertices, uint32_t size);
		//只开辟内存，不填充数据
		static Ref<VertexBuffer> Create(uint32_t size=0);

	};
	//同上
	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer();
		virtual	void Bind()const = 0;
		virtual void UBind()const = 0;

		//填充数据
		virtual void SetData(const void* buffer, uint32_t size)=0 ;

		//also virtual void SetData(const void* buffer,uint32_t size,uint32_t offset)//都是以字节为单位
		//但是其实如果是针对我们这种，我们直接放在cpu上搜集好，然后一次性传输上去（不需要分多次传输）


		virtual uint32_t GetCount() const = 0;

		static Ref<IndexBuffer> Create(uint32_t count = 0);
		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);

	};

}


