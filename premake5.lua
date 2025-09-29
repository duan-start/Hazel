-- 就是为了引入额外的功能插件，用来控制书写和排版的同一
include "./vendor/bin/premake_customization/solution_items.lua"
include "Dependencies.lua"
--引入设置好的宏命名
workspace "Hazel"
	architecture "x86_64"
	startproject "Hazel-Editor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

-- 这是在vendor/premake里面额外添加的内容
	solution_items
	{
        --里面是具体的排版原则
		".editorconfig"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "vendor/bin"
	include "Hazel/vendor/Box2D"
	include "Hazel/vendor/GLFW"
	include "Hazel/vendor/Glad"
	include "Hazel/vendor/imgui"
	include "Hazel/vendor/yaml-cpp"
group ""

--直接引入对应工程下的premake 设置
include "Hazel"
include "Sandbox"
include "Hazel-Editor"