# Hazel 渲染问题知识库

记录 Hazel 引擎开发中遇到并解决的渲染管线问题。每条包含现象、根因、修复、验证证据与排查方法，便于以后快速定位同类问题。

最近更新：2026-08-30（天空盒渲染链路）

## 目录

1. [天空盒完全不显示：深度函数映射缺失](#问题-1天空盒完全不显示深度函数映射缺失)
2. [FBO 重建后天空盒消失：深度缓冲不再清除](#问题-2fbo-重建后天空盒消失深度缓冲不再清除)
3. [u_InverseVP 读到零矩阵：临时 UBO 生命周期](#问题-3u_inversevp-读到零矩阵临时-ubo-生命周期)
4. [天空盒 UBO 与模型 UBO 撞 binding](#问题-4天空盒-ubo-与模型-ubo-撞-binding)
5. [samplerCube 采样恒黑：SPIR-V 二进制管线缺陷](#问题-5samplercube-采样恒黑spir-v-二进制管线缺陷)
6. [修改 shader 不生效：缓存不失效](#问题-6修改-shader-不生效缓存不失效)
7. [通用调试方法](#通用调试方法)
8. [相关资产说明](#相关资产说明)
9. [遗留问题](#遗留问题)

---

## 问题 1：天空盒完全不显示（深度函数映射缺失）

**现象**：天空盒全屏四边形没有渲染，画面只剩清除色。

**根因**：天空盒材质设置了 `DepthFunc::LessEqual`，但 `Material::Bind()` 的 `glDepthFunc` switch 只处理了 `Less / Equal / Greater`，`LessEqual` 落入 default 变成 `GL_LESS`。天空盒四边形 NDC z=1.0，深度缓冲清为 1.0，`1.0 < 1.0` 恒假 → 全部片元被深度测试剔除。

**修复**：`Hazel/src/Hazel/Renderer/Material.cpp` 的 switch 补全全部 8 种深度函数（`LessEqual` → `GL_LEQUAL`，其余 `GreaterEqual / NotEqual / Always / Never` 一并补上）。

**验证**：帧缓冲回读中心像素，修复后天空盒从不可见变为可见。

**排查方法**：在绘制点 `glGetIntegerv(GL_DEPTH_FUNC, ...)` 查询实际深度函数（期望 0x203=GL_LEQUAL，实际是 GL_LESS=0x201）。

---

## 问题 2：FBO 重建后天空盒消失（深度缓冲不再清除）

**现象**：第一帧天空盒正常，编辑器视口 Resize（1280x720 → 856x591）后天空盒消失，FBO 只剩清除色。

**根因**：天空盒材质设置 `glDepthMask(GL_FALSE)` 且是每帧最后一个绘制，`glDepthMask(FALSE)` 残留到下一帧；`Renderer3D::Clear()` 的 `glClear(GL_DEPTH_BUFFER_BIT)` 在深度写掩码为 FALSE 时是空操作。FBO Resize（`Invalidate`）新建的深度纹理初始内容为 0 → 天空盒 z=1.0 在 `GL_LEQUAL` 下被剔除。

**修复**：`OpenGLRendererAPI::Clear()` 在 `glClear` 前强制 `glDepthMask(GL_TRUE)`；`Init()` 显式 `glClearDepth(1.0f)`。

**验证**：第 30 帧深度回读 `centerDepth` 从 0 → 1，天空盒恢复渲染。

**教训**：任何渲染状态（尤其是深度写掩码）都要在每帧开始时显式复位，不能依赖“上一帧最后一个材质”留下的状态。

---

## 问题 3：u_InverseVP 读到零矩阵（临时 UBO 生命周期）

**现象**：即使深度测试通过，天空盒也只显示纯色/采样方向错误。

**根因**：`MaterialInstance::SetUniformMat4` 创建局部 `Ref<UniformBuffer>`，函数结束即析构；`OpenGLUniformBuffer` 析构里 `glDeleteBuffers` 把 binding 点解绑 → shader 读到的 UBO 是空/零矩阵。

**修复**：UBO 改为 `MaterialInstance` 成员长期持有，`Bind()` 时重新 `glBindBufferBase`。`UniformBuffer` 增加 `Bind()` 虚函数，`OpenGLUniformBuffer` 记录 `m_Binding` 并实现重绑。

---

## 问题 4：天空盒 UBO 与模型 UBO 撞 binding

**现象**：天空盒采样方向错误/与模型矩阵数据冲突。

**根因**：`SkyBox.glsl` 的 Camera UBO 用 `binding = 1`，与 Renderer3D 的 `TransformData`（模型矩阵，binding=1）冲突。

**修复**：天空盒改用独立槽 `binding = 3`（Renderer2D=0，Renderer3D=0/1/2）。

---

## 问题 5：samplerCube 采样恒黑（SPIR-V 二进制管线缺陷）

**现象**：所有 GL 状态都正确——贴图有数据（glGetTexImage 返回蓝色）、unit 0 绑定正确（`GL_TEXTURE_BINDING_CUBE_MAP` = 贴图 ID）、采样器单元=0（`glGetProgramResourceiv(GL_LOCATION)` + `glGetUniformiv`）、无 GL 错误——但 `texture(samplerCube, dir)` 恒返回黑色。同一条管线下 `sampler2D` 采样正常。

**根因**：GLSL → shaderc(Vulkan SPIR-V) → SPIRV-Cross → GLSL → shaderc(OpenGL SPIR-V) → `glShaderBinary` / `glSpecializeShader`（ARB_gl_spirv）这条链路在这台机器的 OpenGL 驱动上把 samplerCube 弄坏（驱动 uniform 反射报告类型为 `GL_SAMPLER_2D`、名字为空，采样结果恒黑）。

**修复**：`OpenGLShader::CreateProgram` 改为经典 `glShaderSource + glCompileShader + glLinkProgram` 直接编译 SPIRV-Cross 生成的 GLSL，绕开 `glShaderBinary`。对所有 shader 生效，兼容性更好。

**验证**：
- 固定方向采样 + unit 0 绑红色 2D 纹理 → sampler2D 输出 (255,25,25)（红+调试色）；
- 换回 samplerCube 并在新 GLSL 编译路径下 → 输出蓝色天空 (99,51,193)（Debug.tga 蓝面+调试色）。

**排查方法**：
- `glGetTexImage` 读回贴图内容，确认数据在 GPU 上；
- `glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP)` 确认绘制时绑定；
- `glGetProgramResourceiv(GL_LOCATION / GL_TYPE)` 与 `glGetUniformiv` 确认采样器单元；
- `glGetActiveUniform` 观察名字/类型反射是否异常（名字为空、类型为 2D 即异常信号）；
- 把 fragment 临时改成固定方向/固定颜色采样，逐段隔离“顶点 vs 片元 vs 采样”。

---

## 问题 6：修改 shader 不生效（缓存不失效）

**现象**：修改 `SkyBox.glsl` 后重启编辑器，行为仍是旧 shader（例如红色调试加不上）。

**根因**：shader 缓存只按文件名（`SkyBox.glsl.cached_*`）存取，从不校验源码内容。

**修复**：缓存文件名加入源码 FNV-1a 哈希（`SkyBox.glsl.<hash>.cached_*`），源码一变必然重编。

**注意**：遇到“改了 shader 没效果”，先确认 `assets/cache/shader/opengl/` 下是否有旧文件；本次问题曾因此被误判为渲染管线问题。

---

## 通用调试方法

- **帧缓冲回读**：绘制后 `glReadPixels` + 统计中心像素/占比，直接判断“画没画出来”。
- **GL 状态转储**：`GL_FRAMEBUFFER_BINDING`、`GL_DRAW_BUFFER0/1`、`GL_READ_BUFFER`、`GL_CURRENT_PROGRAM`、`GL_UNIFORM_BUFFER_BINDING`、`GL_DEPTH_FUNC / GL_DEPTH_WRITEMASK`、`GL_VIEWPORT`、`glGetError`。
- **贴图内容验证**：`glGetTexImage` 读回纹理像素；`glGetTexLevelParameteriv` 检查 cubemap 6 个面的完整性与内部格式一致性。
- **采样器验证**：fragment 临时改成固定方向/固定颜色采样，逐段隔离问题。
- **进程内诊断优于截屏**：PrintWindow / CopyFromScreen 在 DPI 缩放环境下可能取到错误的窗口矩形；控制台多行日志会被截断，长文本建议写文件。
- **shader 缓存**：改 shader 不生效时先删 `assets/cache/shader/opengl/` 下对应文件。

---

## 相关资产说明

- `Hazel-Editor/assets/env/Debug.tga`：2048x1536 的 4x3 调试立方体贴图，±X 红、±Z 蓝、±Y 绿，512x512 面；`OpenGLTextureCube` 按标准 4x3 十字提取。
- 天空盒采用“全屏四边形 + inverse(ViewProjection)”方案（非立方体几何方案），顶点在 NDC z=1.0。
- FBO 颜色附件用 `GL_RGBA + GL_INT` 创建是合法的（不是黑屏原因）。
- 编辑器视口显示链路（`ImGui::Image` → FBO 颜色附件纹理）本身正常；问题都在渲染侧。

---

## 遗留问题

- `Sandbox` 工程编译失败：`Sandbox/premake5.lua` 的 includedirs 缺少 ImGui，`ImguiLayer.h` 找不到 `imgui.h`（与本次渲染问题无关，构建层面的历史问题）。
- `OpenGLTextureCube` 的十字面朝向/翻转未做严格验证（Debug.tga 面颜色可辨识，但真实 HDR 环境贴图的方向可能需要后续校正）。
