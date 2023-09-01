/*!***************************************************************************************
\file			editor-content-browser.cpp
\project
\author			Sean Ngo

\par			Course: GAM250
\par			Section:
\date			01/11/2022

\brief
	Contains function definitions for the editor content browser, where the user can view
	and interact with the items in the assets folder.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "pch.h"

#include "Editor/editor-content-browser.h"
#include "Editor/editor-system.h"
#include "Messaging/message-system.h"
#include "Files/file-system.h"
#include "Files/assets-system.h"
#include "Events/events-system.h"

namespace Copium
{
	namespace
	{
		std::filesystem::path assets = "../PackedTracks/Assets";


		const float padding = 16.f;
		const float thumbnailSize = 96.f;
		float imageAR = 1.f, framePadding = 3.f;
		float cellSize = thumbnailSize + padding;

		//std::filesystem::path currentDirectory;

		std::vector<Texture> icons;
	}

	void EditorContentBrowser::init()
	{
		currentDirectory = &MyFileSystem.get_asset_directory();

		icons = MyAssetSystem.GetResources();
	}

	void EditorContentBrowser::update()
	{
		ImGui::Begin("Content Browser", 0);

		inputs();

		if (currentDirectory->path() != assets)
		{
			if (ImGui::Button("Back"))
			{
				currentDirectory = currentDirectory->get_parent_directory();
			}
		}
		else
		{
			if (ImGui::Button("Reload"))
			{
				MessageSystem::Instance()->dispatch(MESSAGE_TYPE::MT_RELOAD_ASSETS);
			}
		}

		ImGui::SameLine();
		if (ImGui::BeginMenu("Add Assets"))
		{
			if (currentDirectory != nullptr)
			{
				//for (auto& it : sS->getScriptableObjectClassMap())
				//{
				//	if (ImGui::MenuItem(it.first.c_str(), nullptr))
				//	{
				//		std::ofstream oStream(Paths::assetPath + "\\" + it.first + ".so");
				//		oStream.close();
				//		//If deserializable else, create new file, SO,
				//		// Copy the script file but change the extension
				//		//MyAssetSystem.CopyAsset(*soFile, ".asset");
				//	}
				//}

				//for (int i = 0; i < scriptableObjects.size(); i++)
				//{
				//	std::string assetName = scriptableObjects[i];
				//	if (ImGui::MenuItem(assetName.c_str(), nullptr))
				//	{
				//		// Find script in relation to the assetname
				//		std::list<File> scriptFiles = MyFileSystem.get_files_with_extension(".cs");
				//		for (File file : scriptFiles)
				//		{
				//			if (!file.stem().string().compare(assetName))
				//			{
				//				// Copy the script file but change the extension
				//				MyAssetSystem.CopyAsset(file, ".asset");
				//				break;
				//			}
				//		}
				//	}
				//}
			}
			ImGui::EndMenu();
		}

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);

		if (columnCount < 1)
			columnCount = 1;

		ImGuiTableFlags tableFlags = ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_SizingStretchSame
			| ImGuiTableFlags_PreciseWidths | ImGuiTableFlags_NoPadInnerX;

		if (ImGui::BeginTable("Table: Content Browser", columnCount, tableFlags))
		{
			ImGuiTableColumnFlags columnFlags = ImGuiTableColumnFlags_WidthFixed;
			for (int i = 0; i < columnCount; i++)
			{
				std::string str = "Column " + i;
				str += ": Assets";

				ImGui::TableSetupColumn(str.c_str(), columnFlags, cellSize);
			}

			// Only for buttons
			const ImVec4 white(1.f, 1.f, 1.f, 1.f);
			const ImVec4 transparent(0.f, 0.f, 0.f, 0.f);

			// Directory/Folder iterator
			for (auto dirEntry : currentDirectory->get_child_directory())
			{
				if (ImGui::TableGetColumnIndex() >= columnCount - 1)
				{
					ImGui::TableNextRow();
				}

				ImGui::TableNextColumn();

				const auto& path = dirEntry->path();
				auto relativePath = std::filesystem::relative(path, assets);
				std::string fileName = relativePath.filename().string();

				ImGui::PushID(fileName.c_str());
				ImGui::BeginGroup();

				ImTextureID icon = (ImTextureID)(size_t)icons[1].get_object_id();
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
				ImGui::ImageButtonEx(dirEntry->get_id(), icon, { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 }, transparent, white);
				
				//ImGui::PopStyleVar();
				ImGui::PopStyleColor();

				float textWidth = ImGui::CalcTextSize(fileName.c_str()).x;
				float indent = abs((cellSize - textWidth - padding) * 0.5f);
				ImGui::Indent(indent);
				ImGui::Text(fileName.c_str());

				ImGui::EndGroup();
				ImGui::PopID();
			}

			// File iterator
			for (auto& file : currentDirectory->get_files())
			{
				// Ignore meta files
				if (file.get_file_type().fileType == FILE_TYPE::META)
					continue;

				if (ImGui::TableGetColumnIndex() >= columnCount - 1)
				{
					ImGui::TableNextRow();
				}
				ImGui::TableNextColumn();

				ImGui::PushID(file.get_id());
				ImGui::BeginGroup();

				// Get the image icon
				unsigned int objectID = icons[2].get_object_id();

				std::string texturePath, filePath;
				switch (file.get_file_type().fileType)
				{
				case FILE_TYPE::AUDIO:
					break;

				case FILE_TYPE::FONT:
					break;

				case FILE_TYPE::SCENE:
					objectID = icons[0].get_object_id();
					imageAR = 1.f;
					framePadding = 3.f;
					break;

				case FILE_TYPE::SCRIPT:
					break;

				case FILE_TYPE::SHADER:
					break;

				case FILE_TYPE::SPRITE:
					for (auto& texture : MyAssetSystem.GetTextures())
					{
						texturePath = texture.get_file_path();
						if (!file.filePath.string().compare(texturePath))
						{
							objectID = texture.get_object_id();
							float asRatio = texture.get_width() / (float)texture.get_height();
							imageAR = thumbnailSize / ((asRatio > 0.98f && asRatio < 1.f) ? 1.f : asRatio);
							imageAR /= thumbnailSize;
							framePadding = (thumbnailSize - thumbnailSize * imageAR) * 0.5f + 3.f;
							break;
						}
					}
					break;

				case FILE_TYPE::TEXT:
					objectID = icons[2].get_object_id();
					imageAR = 1.f;
					framePadding = 3.f;
					break;
				}

				ImTextureID icon = (ImTextureID)(size_t)objectID;
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, framePadding));

				ImGui::ImageButtonEx(file.get_id(),icon, { thumbnailSize, thumbnailSize * imageAR }, { 0, 1 }, { 1, 0 }, transparent, white);

				if (ImGui::BeginDragDropSource())
				{
					std::string str = file.filePath.string();
					const char* tmpfilePath = str.c_str();
					ImGui::SetDragDropPayload("ContentBrowserItem", tmpfilePath, str.size() + 1);

					ImGui::EndDragDropSource();
				}

				ImGui::PopStyleVar();
				ImGui::PopStyleColor();

				float textWidth = ImGui::CalcTextSize(file.get_name().c_str()).x;
				float indent = (cellSize - textWidth - padding) * 0.5f;
				ImGui::Indent(indent);
				ImGui::Text(file.get_name().c_str());

				ImGui::EndGroup();
				ImGui::PopID();
			}

			ImGui::EndTable();
		}

		ImGui::End();
	}

	void EditorContentBrowser::exit()
	{

	}

	void EditorContentBrowser::inputs()
	{
		if (!ImGui::IsMouseDragging(ImGuiMouseButton_Left) && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			// if (!MyEditorSystem.get_inspector()->getFocused())
			// {
			// 	MyEventSystem->publish(new SetSelectedFileEvent(nullptr));
			// 	MyEventSystem->publish(new SetSelectedDirectoryEvent(nullptr));
			// }
		}

		if (ImGui::IsWindowFocused())
		{
			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				for (File& file : currentDirectory->get_files())
				{
					if (file.get_id() == ImGui::GetHoveredID())
						file.access_file();
				}

				for (Directory* dir : currentDirectory->get_child_directory())
				{
					if (dir->get_id() == ImGui::GetHoveredID())
					{
						currentDirectory = dir;
						MyFileSystem.SetCurrentDir(dir);
					}
				}
			}

			if (!ImGui::IsMouseDragging(ImGuiMouseButton_Left) && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				for (File& file : currentDirectory->get_files())
				{
					if (file.get_id() == ImGui::GetHoveredID())
						MyEventSystem->publish(new SetSelectedFileEvent(&file));
				}

				for (Directory* dir : currentDirectory->get_child_directory())
				{
					
					if (dir->get_id() == ImGui::GetHoveredID())
						MyEventSystem->publish(new SetSelectedDirectoryEvent(dir));
				}
			}
			
			if (ImGui::IsKeyPressed(ImGuiKey_Delete))
				MyEventSystem->publish(new DeleteFromBrowserEvent);
		}
	}
}