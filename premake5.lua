workspace "Hazel"  
    architecture "x64"  

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

-- Include directories relative to root folder (Solution directory)  
include "Hazel/vendor/GLFW"  
include "Hazel/vendor/Glad"  
include "Hazel/vendor/imgui"

project "Hazel"  
    location "Hazel"  
    kind "SharedLib"  
    language "C++"  



    targetdir("bin/" ..outputdir.. "/%{prj.name}")  
    objdir ("bin-int/" ..outputdir.. "/%{prj.name}")  

    pchheader "hzpch.h"
    pchsource "Hazel/src/hzpch.cpp"

    files{  
        "%{prj.name}/src/**.h",  
        "%{prj.name}/src/**.cpp"  
    }  

    includedirs{  
        "Hazel/vendor/spdlog/include",
        "Hazel/src",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.Imgui}"
    }

    links{
        "GLFW",
        "Glad",
        "Imgui",
        "opengl32.lib"
    }

    filter "system:windows"  
        cppdialect"C++17"  
        staticruntime"On"  
        systemversion"10.0"  

        defines{  
            "HZ_PLATFORM_WINDOWS",  
            "HZ_BUILD_DLL"  
        }
        postbuildcommands{  
            ("{COPYFILE} %{cfg.buildtarget.relpath} ../bin/" ..outputdir.. "/Sandbox")  
        }

          

    filter "configurations:Debug"  
        defines{"HZ_DEBUG"}  
        symbols"On"  
        buildoptions"/MDd"

    filter "configurations:Release"  
        defines{"HZ_RELEASE"}  
        optimize"On"  
        buildoptions"/Md"

    filter "configurations:Dist"  
        defines{"HZ_DIST"}  
        optimize"On"  
        buildoptions"/Md"

project "Sandbox"  
    location "Sandbox"  
    kind "ConsoleApp"  
    language "C++"  

    targetdir("bin/" ..outputdir.. "/%{prj.name}")  
    objdir("bin-int/" ..outputdir.. "/%{prj.name}")  

    files{  
        "%{prj.name}/src/**.h",  
        "%{prj.name}/src/**.cpp"  
    }  

    includedirs{  
        "Hazel/vendor/spdlog/include",  
        "Hazel/src"  
    }  
    
    links{  
        "Hazel"  
    }  

    filter "system:windows"  
        cppdialect"C++17"  
        staticruntime"On"  
        systemversion"10.0"  

        defines{  
            "HZ_PLATFORM_WINDOWS"
        } 
        

    filter "configurations:Debug"  
        defines{"HZ_DEBUG"}  
        symbols"On"  

    filter "configurations:Release"  
        defines{"HZ_RELEASE"}  
        optimize"On"  

    filter "configurations:Dist"  
        defines{"HZ_DIST"}  
        optimize"On" 