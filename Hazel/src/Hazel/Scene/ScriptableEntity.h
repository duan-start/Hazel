#pragma once  
#include "Entity.h"  

namespace Hazel {  
	//客户端应该是这个的子类
class ScriptableEntity {  
	//拥有而不是继承，尽量解耦合  
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