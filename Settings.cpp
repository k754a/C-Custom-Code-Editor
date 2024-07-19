#include "libraries.h"
#include "Settings.h"

//global

bool settings = false;

void Settingsrender()
{
  
    // Quality of life will make it so you can turn it off
    ImVec4 bgcolor = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
    bgcolor.w = 0.5f; // color value

    // Set the window transparent
    ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = bgcolor;

    // Window with a close button
     if (ImGui::Begin("Settings", &settings, ImGuiWindowFlags_NoCollapse))
        {
            if (ImGui::BeginTabBar("SettingsTabBar"))
            {
                // Create the first tab
                if (ImGui::BeginTabItem("Editor"))
                {
                    ImGui::Text("NULL");
                    ImGui::EndTabItem();
                }

                // Create the second tab
                if (ImGui::BeginTabItem("Project"))
                {
                    ImGui::Text("NULL");
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Debug"))
                {
                    ImGui::Text("NULL");
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Documentation"))
                {
                    ImGui::Text("NULL");
                    ImGui::EndTabItem();
                }

                // End the tab bar
                ImGui::EndTabBar();
            }

            ImGui::End();
        }
    
   
}