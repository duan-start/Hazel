#pragma once
namespace Hazel {
	class UUID {
	public:
		UUID();
		//д�˾Ͳ����Զ������ƶ��ຯ��
		UUID(uint64_t);
		UUID(const UUID& other) = default;
		operator uint64_t() const { return m_UUID; }
	private:
		uint64_t m_UUID;
	};
}

//����uuid�Ͷ�Ӧ�ʲ���һ��һ�Ĺ�ϵ
//unordered_map
//��Ҫ�ж�key�Ƿ���ȵĺ�����������
namespace std{
	template<>
	struct hash<Hazel::UUID> 
	{
		size_t operator()(const Hazel::UUID& uuid) const{
			//�º�����ʵ����
			return hash<uint64_t>()((uint64_t)uuid);
		}
		
	};

}