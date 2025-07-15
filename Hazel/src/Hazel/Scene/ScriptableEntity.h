#pragma once  
#include "Entity.h"  

namespace Hazel {  
class ScriptableEntity {  
	//ӵ�ж����Ǽ̳У����������  
public:  
	template<typename T>  
	T& GetComponent() { return m_Entity.GetComponent<T>(); }  
private:  
	Entity m_Entity;  
	friend class Scene;  
};  
}