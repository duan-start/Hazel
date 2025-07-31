#pragma once

#include "Scene.h"


namespace Hazel {

	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);
		bool Serialize(const std::string& filepath);
		bool SerializeRuntime(const std::string& filepath);

		bool Deserialize(const std::string& filepath);
		bool DeserializeRuntime(const std::string& filepath);
	private:
		Ref<Scene> m_Scene;
	};
}

