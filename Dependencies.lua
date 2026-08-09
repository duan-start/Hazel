
-- Hazel Dependencies

--操作系统获得Vulkan_sdk的具体位置
VULKAN_SDK = os.getenv("VULKAN_SDK")

--Intialize IncludeDires
--%{}会去找系统的环境变量（类似在终端里面的访问）
IncludeDir = {}
IncludeDir["stb_image"] = "%{wks.location}/Hazel/vendor/stb_image"
IncludeDir["assimp"] = "%{wks.location}/Hazel/vendor/assimp/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/Hazel/vendor/yaml-cpp/include"
IncludeDir["Box2D"] = "%{wks.location}/Hazel/vendor/Box2D/include"
IncludeDir["GLFW"] = "%{wks.location}/Hazel/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Hazel/vendor/Glad/include"
IncludeDir["mono"] = "%{wks.location}/Hazel/vendor/mono/include"
IncludeDir["ImGui"] = "%{wks.location}/Hazel/vendor/imgui"
IncludeDir["ImGuizmo"] = "%{wks.location}/Hazel/vendor/ImGuizmo"
IncludeDir["glm"] = "%{wks.location}/Hazel/vendor/glm"
IncludeDir["entt"] = "%{wks.location}/Hazel/vendor/Entt/include"
IncludeDir["shaderc"] = "%{wks.location}/Hazel/vendor/shaderc/include"
IncludeDir["SPIRV_Cross"] = "%{wks.location}/Hazel/vendor/SPIRV-Cross"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"


--Initialized Lidrary,指定原始的lib和编译到本地项目的debug版本
LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["assimp"] = "%{wks.location}/Hazel/vendor/assimp"
LibraryDir["mono"] = "%{wks.location}/Hazel/vendor/mono/lib/%{cfg.buildcfg}"
--利用上面已经定义好的去找到对应的库
Library = {}
Library["Mono"] = "%{LibraryDir.mono}/libmono-static-sgen.lib"
Library["Assimp"]="%{LibraryDir.assimp}/Libs/assimp-vc143-mtd.lib"
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"

Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK}/SPIRV-Toolsd.lib"

Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"

-- Windows
Library["WinSock"] = "Ws2_32.lib"
Library["WinMM"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["BCrypt"] = "Bcrypt.lib"