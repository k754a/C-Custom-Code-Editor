#include "libraries.h"
#include "Settings.h"

#include <Windows.h>

#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions
#include <vector>
#include "Systemstat.h"
#include <sysinfoapi.h>


//global
#include "getinfo.h"









bool settings = false;

ImVec4 warningc = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

bool devmode, rendergraph = false;

int c;

bool winfpsread = false;
constexpr int MAX_HISTORY_SIZE = 100;
std::vector<float> cpuHistory(MAX_HISTORY_SIZE, 0.0f);
std::vector<float> ramHistory(MAX_HISTORY_SIZE, 0.0f);
int historyIndex = 0;

void UpdateUsageData() {
    static float cpuUsage = 0.0f;
    static float ramUsage = 0.0f;

    cpuUsage = GetCPUUsage();
    ramUsage = GetRAMUsage();

    cpuHistory[historyIndex] = cpuUsage;
    ramHistory[historyIndex] = ramUsage;

    historyIndex = (historyIndex + 1) % MAX_HISTORY_SIZE;
}



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

    ImGui::SetWindowFocus("Settings");
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
                            settings = false;
                        }
                        else {
                            std::cout << "File does not exist.\n";
                        }
                    }
                    ImGui::PopStyleColor();//this prevents crashes

                    //developer mode
                    if (ImGui::Button("Developer Mode"))
                    {
                        if (devmode)
                        {
                            devmode = false;
                        }
                        else
                        {
                            devmode = true;
                        }
                    }
               

             

                    ImGui::Separator();

                    ImGui::EndTabItem();
                }

                // Create the second tab
                if (ImGui::BeginTabItem("Current files"))
                {
                   

                    ImGui::EndTabItem();
                }

                if (devmode == true)
                {
                    if (ImGui::BeginTabItem("Developer"))
                    {
                       
                        if (ImGui::Button("Enable system resource display"))
                        {
                            rendergraph = true;
                           

                        }

                        if (rendergraph) {
                            // Update usage data for graph display
                            

                            if (c < 60)
                            {
                                c++;
                            }
                    
                            else
                            {
                                UpdateUsageData();
                               
                                c = 0;
                            }
                            ImGui::PlotLines("CPU Usage", cpuHistory.data(), cpuHistory.size(), 0, nullptr, 0.0f, 100.0f, ImVec2(0, 80));
                            ImGui::PlotLines("RAM Usage", ramHistory.data(), ramHistory.size(), 0, nullptr, 0.0f, 100.0f, ImVec2(0, 80));

                         
                            

                     
                        }
                        if (!winfpsread)
                        {
                            if (ImGui::Button("Enable Window FPS reader"))
                            {
                                //yay :)
                                winfpsread = !winfpsread;
                            }
                        }
                        else
                        {

                            if (ImGui::Button("Disable Window FPS reader"))
                            {
                                winfpsread = !winfpsread;
                            }

                        }


                        if (ImGui::Button("Open Style folder"))
                        {
                            ShellExecute(NULL, L"open", L"Style", NULL, NULL, SW_RESTORE);
                        }

                        ImGui::EndTabItem();
                    }

                }

                else {
                    if (ImGui::BeginTabItem("Debug"))
                    {
                        ImGui::Text("Save statistics data? This is if you have an issue");
                        if (ImGui::Button("Yes"))
                        {
                            std::ofstream outFile("DEBUGTEXT.LOG");

                            outFile << "NOTE: This is not shared with anyone, if you have issues this could help if something is wrong" << std::endl;
                            outFile << "Computer Name: " << getComputerName() << std::endl;
                            outFile << "CPU Model: " << getCPUInfo() << std::endl;
                            outFile << "RAM Info: " << getRAMInfo() << std::endl;
                            outFile << "Storage Info: " << getStorageInfo() << std::endl;
                            outFile << "cpu usage history" << cpuHistory.data() << std::endl;
                            outFile << "ram usage history" << ramHistory.data() << std::endl; 
                            outFile << "cpu usage size" << cpuHistory.size() << std::endl;
                            outFile << "ram usage size" << ramHistory.size() << std::endl;
                            // Close the file
                            outFile.close();

                            std::cout << "File created and written successfully." << std::endl;

                          
                        }
                        ImGui::EndTabItem();
                    }
                }
                

                if (ImGui::BeginTabItem("Documentation"))
                {
                    ImGui::Text("This is the link to my documentation down below!");
                    const char* url = "https://k754a.github.io/Editor%20Documentation";
                    ImGui::TextColored(ImVec4(0.0f, 0.0f, 1.0f, 1.0f), "Editor Documentation");
                    if (ImGui::IsItemClicked())
                    {
                        // Convert const char* to std::wstring
                        int len;
                        int urlLength = strlen(url) + 1;
                        len = MultiByteToWideChar(CP_ACP, 0, url, urlLength, NULL, 0);
                        std::wstring wideUrl(len, L'\0');
                        MultiByteToWideChar(CP_ACP, 0, url, urlLength, &wideUrl[0], len);

                        ShellExecute(0, 0, wideUrl.c_str(), 0, 0, SW_SHOW);
                    }


                    ImGui::EndTabItem();
                }

                // End the tab bar
                ImGui::EndTabBar();
            }

            ImGui::End();
     }
    
   
}