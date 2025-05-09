workspace "Hazel"  
    architecture "x64"  
    startproject "Sandbox"
    
    configurations{  
        "Debug",  
        "Release",  
        "Dist"  
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"  


IncludeDir = {}  
IncludeDir["GLFW"] = "Hazel/vendor/GLFW/include"  
IncludeDir["Glad"] = "Hazel/vendor/Glad/include"  
IncludeDir["Imgui"] = "Hazel/vendor/imgui"
IncludeDir["glm"] = "Hazel/vendor/glm"
IncludeDir["stb_image"] = "Hazel/vendor/stb_image"



-- Include directories relative to root folder (Solution directory)  
--这是工程文件啊，下面的是在干嘛啊，肯定不能加啊，有没有premakefile文件，肯定不行
--include "./Hazel/vendor/glm"
--print("GLM Include Directory: " .. IncludeDir["glm"])
group "Dependencies"
        include "Hazel/vendor/GLFW"
        include "Hazel/vendor/Glad"
        include "Hazel/vendor/imgui"


 -- 定义 Core 组
group "Core"

project "Hazel"  
    location "Hazel"  
    kind "StaticLib"  
    language "C++"  
    cppdialect"C++17"  
    staticruntime "on"



    targetdir("bin/" ..outputdir.. "/%{prj.name}")  
    objdir ("bin-int/" ..outputdir.. "/%{prj.name}")  

    pchheader "hzpch.h"
    pchsource "Hazel/src/hzpch.cpp"

    files{  
        "%{prj.name}/src/**.h",  
        "%{prj.name}/src/**.cpp",
        "%{IncludeDir.glm}/glm/**.hpp",
        "%{IncludeDir.glm}/glm/**.inl",
        "%{IncludeDir.stb_image}/**.h",
        "%{IncludeDir.stb_image}/**.cpp"
    }  

    includedirs{  
        "Hazel/vendor/spdlog/include",
        "Hazel/src",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.Imgui}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb_image}"
    }

    links{
        "GLFW",
        "Glad",
        "Imgui",
        "opengl32.lib"
    }

    filter "system:windows"  
        systemversion"10.0"  

        defines{  
            "HZ_PLATFORM_WINDOWS",  
            "HZ_BUILD_DLL",
            "_CRT_SECURE_NO_WARNINGS"
        }

    

    filter "configurations:Debug"  
        defines{"HZ_DEBUG"}  
        runtime "Debug"
        symbols"On"  
        

    filter "configurations:Release"  
        defines{"HZ_RELEASE"}  
        runtime "Release"
        optimize"On"  
      

    filter "configurations:Dist"  
        defines{"HZ_DIST"}  
        runtime "Release"
        optimize"On"  
        
  -- 定义 Applications 组
group "Applications"
project "Sandbox"  
    location "Sandbox"  
    kind "ConsoleApp"  
    language "C++"  
    cppdialect"C++17"  
    staticruntime "on"

    targetdir("bin/" ..outputdir.. "/%{prj.name}")  
    objdir("bin-int/" ..outputdir.. "/%{prj.name}")  

    files{  
        "%{prj.name}/src/**.h",  
        "%{prj.name}/src/**.cpp"  
    }  

    includedirs{  
        "Hazel/vendor/spdlog/include",  
        "Hazel/vendor/imgui",
        "Hazel/vendor/glm",
        "Hazel/vendor/stb_image",
        "Hazel/src"  
    }  
    
    links{  
        "Hazel"  
    }  

    filter "system:windows"  
        systemversion"10.0"  

        defines{  
            "HZ_PLATFORM_WINDOWS"
        } 
        

    filter "configurations:Debug"  
        defines{"HZ_DEBUG"} 
        runtime "Debug"
        symbols"on"  

    filter "configurations:Release"  
        defines{"HZ_RELEASE"}  
        runtime "Release"
        optimize"on"  

    filter "configurations:Dist"  
        defines{"HZ_DIST"}  
        runtime "Release"
        optimize"on" 