#pragma once
namespace Hazel {
	//随机数，资产的handle
	class UUID {
	public:
		UUID();
		//写了就不会自动生成移动类函数
		UUID(uint64_t);
		UUID(const UUID& other) = default;

		//隐式转化
		operator uint64_t() const { return m_UUID; }
	private:
		uint64_t m_UUID;
	};
}

//由于uuid和对应资产是一对一的关系(unordered_map的底层是hash_key,而且对于资源管理一般用的都是这个容器)
//所以需要重新定义这个类的hash偏特化
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