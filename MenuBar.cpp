
#include "shaderclass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "MenuBar.h"
#include"Texture.h"
#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

bool close = false;


void RenderMenuBar()
{


    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            
            ImGui::Separator();
            if (ImGui::MenuItem("Exit"))
            {
                close = true;
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