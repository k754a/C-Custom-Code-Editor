
#include "MenuBar.h"

void RenderMenuBar()
{

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            
            ImGui::Separator();
            if (ImGui::MenuItem("Exit"))
            {
                
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo"))
            {
                
            }
            if (ImGui::MenuItem("Redo"))
            {
                
            }
            ImGui::Separator();
            
            ImGui::EndMenu();
        }
      

        ImGui::EndMainMenuBar();
    }
}