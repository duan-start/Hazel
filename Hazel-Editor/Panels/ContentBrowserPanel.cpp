#include "ContentBrowserPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Hazel {
	
	//固定的，而不依赖于外界
	//const 默认是staic,需要显示extern（给外部使用）
	extern const std::filesystem::path g_AssetPath = "assets";

	ContentBrowserPanel::ContentBrowserPanel():
		m_CurrentDirectory(g_AssetPath)
	{
		//创建图标纹理
		m_DirectoryIcon = Texture2D::Create("Resources/Icons/ContentBrowser/DirectoryIcon.png");
		m_FileIcon = Texture2D::Create("Resources/Icons/ContentBrowser/FileIcon.png");
	}
	//imgui 的使用和用法
	void ContentBrowserPanel::OnImguiRenderer()
	{
		//Reset
		static float padding = 68.f;
		static float thumbnailSize = 128.0f;
		float cellSize = thumbnailSize + padding;
		//ImGuiStyle& style = ImGui::GetStyle();
		//style.ItemSpacing.x = 0.0f; // 去掉列间默认间距

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Begin("Content Browser");


		if (m_CurrentDirectory != std::filesystem::path(g_AssetPath))
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}
		ImGui::Columns(columnCount - 2, nullptr, false);
		//Draw,遍历

		//在当前目录下的所有的文件
		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{

			//计算实际路径
			const auto& path = directoryEntry.path();
			//ralativePath 去掉更目录和盘，也就是计算相对目录
			auto relativePath = std::filesystem::relative(path, g_AssetPath);
			//相对路径当做这个Imgui绘制图标对应的ID
			std::string filenameString = relativePath.filename().string();
			ImGui::PushID(filenameString.c_str());

			//图标绘制
			//对于目录和文件采取不同的绘制方式
			Ref<Texture2D> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
			
			//背景颜色变透明
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton((ImTextureID)icon->GetRenderID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
			ImGui::PopStyleColor();

			//Core-,,payload 是路径，然后“”这个是一个匹配的flag
			//实现拖拽
			if (ImGui::BeginDragDropSource())
			{
				const wchar_t* itemPath = relativePath.c_str();
				//设置文件相对路径
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}



			//悬停并点击两次-->进入目录
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					m_CurrentDirectory /= path.filename();
			}

			//自动换行
			ImGui::TextWrapped(filenameString.c_str());

			//下一个格子（或者是右边的格子）
			ImGui::NextColumn();
			ImGui::PopID();
		}

		//回到单列模式（整体控件布局的设置）
		ImGui::Columns(1);
		//调整
		ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
		ImGui::SliderFloat("Padding", &padding, 0, 128);

		ImGui::End();
	}

}
