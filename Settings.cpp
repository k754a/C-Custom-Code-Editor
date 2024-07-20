#include "libraries.h"
#include "Settings.h"

//global

bool settings = false;
ImVec4 warningc = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);


//init file system

//namespace fs = std::filesystem;

void Settingsrender()
{
  
    // Quality of life will make it so you can turn it off
    ImVec4 bgcolor = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
    float windowWidth = ImGui::GetWindowSize().x;
    float textPosX = (windowWidth - ImGui::GetFontSize()) * 0.5f; //mid centerd
    ImVec2 txSize = ImGui::CalcTextSize("Current directory's open");// his gets the size of the text!
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
                     /*txSize = ImGui::CalcTextSize("Current directory's open");// his gets the size of the text!
                    textPosX = (windowWidth - txSize.x) * 0.5f; // mid centered
                    ImGui::SetCursorPosX(textPosX);
                    ImGui::Text("Current directory's Open");*/

                    ImGui::Separator();
                    windowWidth = ImGui::GetWindowSize().x;
                    textPosX = (windowWidth - ImGui::GetFontSize()) * 0.5f; //mid centerd

                    ImGui::SetCursorPosX(textPosX);

                    ImGui::TextColored(warningc, "Warning");

                   
                    windowWidth = ImGui::GetWindowSize().x;
                    //find the textsize
                     txSize = ImGui::CalcTextSize("Changing any of the settings below could cause issues or instability");//this gets the size of the text!
                    textPosX = (windowWidth - txSize.x) * 0.5f; // mid centered
                    ImGui::SetCursorPosX(textPosX);

                    ImGui::Text("Changing any of the settings below could cause issues or instability");
                   
                    ImGui::Separator();
                    //clear global memory
                    //button color
                    ImGui::PushStyleColor(ImGuiCol_Text, warningc);//make it red

                    if (ImGui::Button("Clear Global Memory"))
                    {
                        if (std::remove(".\\Psettings\\CfilePath.FUNCT")) {
                            std::cout << "File deleted successfully.\n";
                        }
                        else {
                            std::cout << "File does not exist.\n";
                        }
                    }
                    ImGui::PopStyleColor();//this prevents crashes
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