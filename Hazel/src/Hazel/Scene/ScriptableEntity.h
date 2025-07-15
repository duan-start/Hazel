#pragma once  
#include "Entity.h"  

namespace Hazel {  
class ScriptableEntity {  
	//拥有而不是继承，尽量解耦合  
public:  
	template<typename T>  
	T& GetComponent() { return m_Entity.GetComponent<T>(); }  
private:  
	Entity m_Entity;  
	friend class Scene;  
};  
}