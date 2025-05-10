#pragma once

//for used by hazel application

#include "Hazel/Core/Timestep.h"

#include "Hazel/Core/Application.h"
#include "Hazel/Core/Log.h"
#include "Hazel/Core/Layer.h"
#include "Hazel/Imgui/ImguiLayer.h"

//#include "Hazel/Input.h"
//#include "Hazel/KeyCodes.h"
//#include "Hazel/MouseButtonCodes.h"
#include "Hazel/Renderer/OrthographicCameraController.h"

#include "Hazel/Renderer/Buffer.h"
#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/Texture.h"
#include "Hazel/Renderer/VertexArray.h"

#include "Hazel/Renderer/Renderer.h"
#include "Hazel/Renderer/RenderCommand.h"

#include "Hazel/Renderer/OrthographicCamera.h"
#include "Hazel/Renderer/PerspectivegraphicCamera.h"

//my_add
#include <glm/gtc/matrix_transform.hpp >
#include <glm/gtc/type_ptr.hpp>
//---Entity Point-------------
//#include"Hazel/Core/EntryPoint.h"
//因为这个是放在src文件中的，所以和Hazel是同级目录，头文件一般是这样的，源文件的话记住在属性的附加依赖项。