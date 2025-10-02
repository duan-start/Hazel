#pragma once
namespace Hazel {
	class UUID {
	public:
		UUID();
		//写了就不会自动生成移动类函数
		UUID(uint64_t);
		UUID(const UUID& other) = default;
		operator uint64_t() const { return m_UUID; }
	private:
		uint64_t m_UUID;
	};
}

//由于uuid和对应资产是一对一的关系
//unordered_map
//需要判断key是否相等的函数，在这里
namespace std{
	template<>
	struct hash<Hazel::UUID> 
	{
		size_t operator()(const Hazel::UUID& uuid) const{
			//仿函数的实例化
			return hash<uint64_t>()((uint64_t)uuid);
		}
		
	};

}