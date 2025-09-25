#include "ContentBrowserPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Hazel {
	
	//�̶��ģ��������������
	//const Ĭ����staic,��Ҫ��ʾextern
	extern const std::filesystem::path g_AssetPath = "assets";

	ContentBrowserPanel::ContentBrowserPanel():
		m_CurrentDirectory(g_AssetPath)
	{
		m_DirectoryIcon = Texture2D::Create("Resources/Icons/ContentBrowser/DirectoryIcon.png");
		m_FileIcon = Texture2D::Create("Resources/Icons/ContentBrowser/FileIcon.png");
	}
	//imgui ��ʹ�ú��÷�
	void ContentBrowserPanel::OnImguiRenderer()
	{
		//Reset
		static float padding = 0.f;
		static float thumbnailSize = 128.0f;
		float cellSize = thumbnailSize + padding;


		//ImGuiStyle& style = ImGui::GetStyle();
		//style.ItemSpacing.x = 0.0f; // ȥ���м�Ĭ�ϼ��

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Begin("Content Browser");
		ImGui::Columns(columnCount*2, nullptr, false);



		if (m_CurrentDirectory != std::filesystem::path(g_AssetPath))
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		//Draw,����
		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			//ralativePath ȥ����Ŀ¼���̣�Ҳ���Ǽ������Ŀ¼
			auto relativePath = std::filesystem::relative(path, g_AssetPath);

			std::string filenameString = relativePath.filename().string();
			ImGui::PushID(filenameString.c_str());
			Ref<Texture2D> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton((ImTextureID)icon->GetRenderID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
			ImGui::PopStyleColor();

			//Core-,,payload ��·����Ȼ�󡰡������һ��ƥ���flag
			if (ImGui::BeginDragDropSource())
			{
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					m_CurrentDirectory /= path.filename();
			}
			ImGui::TextWrapped(filenameString.c_str());

			ImGui::NextColumn();
			ImGui::PopID();
		}

		//�ص�����ģʽ
		ImGui::Columns(1);
		//����
		ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
		ImGui::SliderFloat("Padding", &padding, 0, 32);

		ImGui::End();
	}

}
