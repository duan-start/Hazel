#pragma once  
#include "Entity.h"  

namespace Hazel {  
	//�ͻ���Ӧ�������������
class ScriptableEntity {  
	//ӵ�ж����Ǽ̳У����������  
public:  
	template<typename T>  
	T& GetComponent() { return m_Entity.GetComponent<T>(); }  

	virtual ~ScriptableEntity() = default;
protected:
	virtual void OnCreate() = 0;
	virtual void OnUpdate(Timestep ts) = 0;
	virtual void OnDestroy() = 0;

private:  
	Entity m_Entity;  
	friend class Scene;  
};  
}