#include "hzpch.h"
#include "UUID.h"

#include <random>

#include <unordered_map>

namespace Hazel {
	//用真的随机数进行伪随机化
	//固定的一个种子，可以生成多个随机数
	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_Engine(s_RandomDevice());

	//将上述的输出映射分布到一个均匀的空间，（这个均匀的空间就是uint64_t的空间，因为默认的mt19937和我们范围不一定相同
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

	UUID::UUID()
		: m_UUID(s_UniformDistribution(s_Engine))
	{
	}

	UUID::UUID(uint64_t uuid)
		: m_UUID(uuid)
	{
	}
}