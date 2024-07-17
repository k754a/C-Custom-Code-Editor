#include"headbar.h"


void Renderbar()
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
            if (ImGui::MenuItem("New"))
            {

            }
            if (ImGui::MenuItem("ObjectEdit window"))
            {

            }
            if (ImGui::MenuItem("Close All Windows"))
            {

            }

            ImGui::Separator();

            ImGui::EndMenu();
        }


        ImGui::EndMainMenuBar();
    }
}