#pragma once

#include "Scene.h"


namespace Hazel {
	//场景序列化  超级重要（）
	class SceneSerializer
	{
	public:
		//保存对当前reference的引用
		SceneSerializer(const Ref<Scene>& scene);

		//序列化到对应的路径
		bool Serialize(const std::string& filepath);
		bool SerializeRuntime(const std::string& filepath);

		//从对应的路径中读取
		bool Deserialize(const std::string& filepath);
		bool DeserializeRuntime(const std::string& filepath);
	private:
		Ref<Scene> m_Scene;
	};
}

