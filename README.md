# Hazel

一个基于 C++17 + OpenGL 4.5 的实时渲染引擎与场景编辑器，当前以 Windows 为主要开发平台。

项目最初基于 TheCherno/Hazel 的教学架构，之后持续扩展了 HDR 渲染管线、天空盒、后处理、材质系统、模型导入和 ShaderToy 视口等模块。

## 功能

### 渲染

- 3D 网格渲染（Assimp 模型导入）
- 2D 批处理渲染：Quad / Circle / Line
- HDR 天空盒
  - Radiance `.hdr`（等距柱状图，自动转换为 cubemap）
  - 4×3 十字布局 `.tga`
- HDR 显示链
  - RGB16F HDR 环境纹理
  - RGBA16F 场景帧缓冲
  - ACES tone mapping + gamma 校正
  - RGBA8 显示缓冲
- Exposure 实时调节
- ShaderToy 风格视口
  - 直接使用 `mainImage()` 形式的 shader
  - 提供 `iResolution` / `iTime` / `iTimeDelta` / `iFrame` / `iMouse` / `iDate`
  - 预留 `iChannel0..3` 采样器

### 编辑器

- Docking 布局
- Scene Hierarchy / Inspector / Content Browser
- ImGuizmo 变换操作
- Scene 序列化（YAML）
- Play / Simulate 模式
- 资产拖拽（场景、天空盒、ShaderToy shader）

### 系统

- ECS（EnTT）
- Box2D 2D 物理
- Mono C# 脚本（实验性）
- 渲染问题知识库

## 技术栈

- C++17
- OpenGL 4.5（DSA / UBO / 浮点帧缓冲）
- GLFW + Glad
- Dear ImGui + ImGuizmo
- glm
- EnTT
- yaml-cpp
- Assimp
- Box2D
- Mono（C# 脚本）

## 渲染管线

```
场景内容（3D Mesh / 2D Sprite / Skybox）
            │
            ▼
   场景 Framebuffer（RGBA16F）
   ├── Color Attachment
   ├── Entity ID Attachment（拾取）
   └── Depth Attachment
            │
            ▼
   全屏后处理（Exposure → ACES → Gamma）
            │
            ▼
   显示 Framebuffer（RGBA8）
            │
            ▼
      ImGui Viewport
```

天空盒与场景内容共享同一条 HDR 管线；拾取使用独立 attachment，不受后处理影响。

## 构建

### 环境要求

- Windows 10/11
- Visual Studio 2022，安装「使用 C++ 的桌面开发」工作负载
- Git（建议支持 submodule）

### 获取代码

```bash
git clone --recursive https://github.com/duan-start/Hazel.git
cd Hazel
git submodule update --init --recursive
```

### 生成工程

仓库已包含 `Hazel.sln`，可以直接用 Visual Studio 2022 打开。

需要重新生成时：

```bash
vendor\premake\bin\premake5.exe vs2022
```

### 编译

1. 打开 `Hazel.sln`
2. 将 `Hazel-Editor` 设为启动项目
3. 选择 `Debug | x64`
4. 构建并运行

当前主要维护 `Hazel` 与 `Hazel-Editor` 两个工程；`Sandbox` 为早期示例工程，构建失败属于已知遗留问题。

## 运行

编辑器依赖 `Hazel-Editor/assets` 下的相对资源，工作目录必须是 `Hazel-Editor`：

```bash
cd Hazel-Editor
..\bin\Debug-windows-x86_64\Hazel-Editor\Hazel-Editor.exe
```

## 使用

- 在 Content Browser 中拖拽场景文件到 Viewport 打开场景
- 把 `.hdr` / `.tga` 拖到 Setting 窗口的 `skyBox` 按钮上切换环境贴图
- 用 `Exposure` 滑条调节 HDR 显示亮度
- 在 ShaderToy 窗口中：
  - 把只包含 `mainImage()` 的 `.glsl` 文件拖到画面上加载
  - 修改文件后点击 `Reload`
  - `Play` 控制时间推进

ShaderToy 源文件不需要声明 uniform，引擎会自动注入协议前缀和 `main()` 入口。示例见 `Hazel-Editor/assets/shaders/ShaderToy/default.glsl`。

## 目录结构

```
Hazel/
├── Hazel/                 引擎核心（渲染、场景、ECS、资产、脚本）
├── Hazel-Editor/          编辑器应用、UI 面板与运行时资源
├── Hazel-ScriptCore/      C# 脚本 API
├── Sandbox/               早期示例工程
├── docs/                  渲染问题知识库与学习笔记
├── vendor/                premake 工具
└── Hazel/vendor/          第三方子模块
```

## 文档

- [渲染问题知识库](docs/rendering-knowledge-base.md)：天空盒黑屏、深度函数、UBO binding、samplerCube、shader 缓存、HDR 面序等真实排查记录。

## 当前状态与路线图

| 模块 | 状态 |
|---|---|
| HDR 天空盒 + 显示链 | 已完成 |
| ShaderToy 视口 | 已完成 |
| PBR 直接光照 | 计划中 |
| IBL（Irradiance / Prefilter / BRDF LUT） | 计划中 |
| Shadow Mapping | 计划中 |
| Bloom | 计划中 |
| 自动化测试 / CI | 计划中 |

## 已知限制

- 目前主要支持 Windows + OpenGL，Vulkan 后端尚未完成
- Shader 编译失败时仍可能触发断言，错误 UI 尚未接入
- ShaderToy 的 `iChannel0..3` 还没有绑定默认纹理
- 仓库包含较大的 HDR / 模型资产，克隆体积较大

## License

本项目基于 Apache License 2.0 发布，详见 [LICENSE](LICENSE)。
