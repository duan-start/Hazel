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

-- Include directories relative to root folder (Solution directory)  
include "Hazel/vendor/GLFW"  
include "Hazel/vendor/Glad"  
include "Hazel/vendor/imgui"
--这是工程文件啊，下面的是在干嘛啊，肯定不能加啊，有没有premakefile文件，肯定不行
--include "./Hazel/vendor/glm"

--print("GLM Include Directory: " .. IncludeDir["glm"])


project "Hazel"  
    location "Hazel"  
    kind "SharedLib"  
    language "C++"  
    staticruntime "off"



    targetdir("bin/" ..outputdir.. "/%{prj.name}")  
    objdir ("bin-int/" ..outputdir.. "/%{prj.name}")  

    pchheader "hzpch.h"
    pchsource "Hazel/src/hzpch.cpp"

    files{  
        "%{prj.name}/src/**.h",  
        "%{prj.name}/src/**.cpp",
        "%{IncludeDir.glm}/glm/**.hpp",
        "%{IncludeDir.glm}/glm/**.inl"
    }  

    includedirs{  
        "Hazel/vendor/spdlog/include",
        "Hazel/src",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.Imgui}",
        "%{IncludeDir.glm}"
    }

    links{
        "GLFW",
        "Glad",
        "Imgui",
        "opengl32.lib"
    }

    filter "system:windows"  
        cppdialect"C++17"  
        systemversion"10.0"  

        defines{  
            "HZ_PLATFORM_WINDOWS",  
            "HZ_BUILD_DLL"  
        }

        postbuildcommands{
            -- 创建目标文件夹（如果不存在）
            ("{MKDIR} \"../bin/" .. outputdir .. "/Sandbox\""),
            -- 复制 Hazel.dll 到 Sandbox 的输出目录
            ("{COPYFILE} %{cfg.buildtarget.abspath} \"../bin/" .. outputdir .. "/Sandbox\"")
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
        

project "Sandbox"  
    location "Sandbox"  
    kind "ConsoleApp"  
    language "C++"  
    staticruntime "off"

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
        "Hazel/src"  
    }  
    
    links{  
        "Hazel"  
    }  

    filter "system:windows"  
        cppdialect"C++17"  
        systemversion"10.0"  

        defines{  
            "HZ_PLATFORM_WINDOWS"
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