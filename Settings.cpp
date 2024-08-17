#include "imgui.h"
#include "imgui_internal.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <windows.h>

bool settings = false;
ImVec4 warningc = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
bool devmode = false, rendergraph = false;
int c = 0;
bool winfpsread = false;
bool darkMode = true;
bool autoSave = false;

constexpr int MAX_HISTORY_SIZE = 100;
std::vector<float> cpuHistory(MAX_HISTORY_SIZE, 0.0f);
std::vector<float> ramHistory(MAX_HISTORY_SIZE, 0.0f);
int historyIndex = 0;

void UpdateUsageData() {
    float cpuUsage = 50.0f;
    float ramUsage = 30.0f;

    cpuHistory[historyIndex] = cpuUsage;
    ramHistory[historyIndex] = ramUsage;

    historyIndex = (historyIndex + 1) % MAX_HISTORY_SIZE;
}

std::string GetCppFilePath() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string exePath(buffer);
    std::string cppFilePath = exePath.substr(0, exePath.find_last_of("\\/"));

    cppFilePath = cppFilePath.substr(0, cppFilePath.find_last_of("\\/"));
    cppFilePath = cppFilePath.substr(0, cppFilePath.find_last_of("\\/"));

    return cppFilePath + "\\Project2\\Docs\\Paged File Setting.html";
}

void OpenHTMLFile(const std::string& filePath) {
    std::string command;

#ifdef _WIN32

    command = "start \"\" \"" + filePath + "\"";
#else

    command = "xdg-open \"" + filePath + "\"";
#endif

    system(command.c_str());
}

void Settingsrender() {
    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    ImVec4 bgcolor = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
    bgcolor.w = 0.9f;
    ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = bgcolor;

    ImGui::SetWindowFocus("Settings");
    if (ImGui::Begin("Settings", &settings, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize)) {
        if (ImGui::BeginTabBar("SettingsTabBar")) {

            if (ImGui::BeginTabItem("Editor")) {
                ImGui::Separator();
                ImGui::TextColored(warningc, "Warning");
                ImGui::TextWrapped("Changing any of the settings below could cause issues or instability.");

                ImGui::Separator();
                if (ImGui::Button("[x] Clear Global Memory")) {
                    if (std::remove(".\\Psettings\\CfilePath.FUNCT") == 0) {
                        std::cout << "File deleted successfully.\n";
                        settings = false;
                    }
                    else {
                        std::cout << "File does not exist.\n";
                    }
                }

                ImGui::Checkbox("Developer Mode", &devmode);
                ImGui::Checkbox("Dark Mode", &darkMode);
                ImGui::Checkbox("Auto-Save", &autoSave);

                if (autoSave)
                {
                    ImGui::Text("Auto-Save is enabled.");
                }
                else
                {
                    ImGui::Text("Auto-Save is disabled.");
                }

                ImGui::Separator();

                ImGui::Checkbox("Paged file Setting", &autoSave);

                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("This setting controls whether files are paged.");
                }

                ImGui::SameLine();
                if (ImGui::SmallButton("What's This?")) {
                    std::string docPath = GetCppFilePath();
                    OpenHTMLFile(docPath);
                }

                ImGui::Separator();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Current Files")) {
                ImGui::Text("This is where you'd list current files.");
                ImGui::EndTabItem();
            }

            if (devmode) {
                if (ImGui::BeginTabItem("Developer")) {
                    if (ImGui::Button("Enable system resource display")) {
                        rendergraph = true;
                    }

                    if (rendergraph) {
                        if (c < 60) {
                            c++;
                        }
                        else {
                            UpdateUsageData();
                            c = 0;
                        }
                        ImGui::PlotLines("CPU Usage", cpuHistory.data(), cpuHistory.size(), 0, nullptr, 0.0f, 100.0f, ImVec2(0, 80));
                        ImGui::PlotLines("RAM Usage", ramHistory.data(), ramHistory.size(), 0, nullptr, 0.0f, 100.0f, ImVec2(0, 80));
                    }

                    ImGui::Checkbox("Enable Window FPS reader", &winfpsread);

                    if (ImGui::Button("Open Style Folder")) {
                        system("start .\\Style");
                    }

                    ImGui::EndTabItem();
                }
            }

            if (!devmode) {
                if (ImGui::BeginTabItem("Debug")) {
                    ImGui::Text("Save statistics data? This is if you have an issue.");
                    if (ImGui::Button("Yes")) {
                        std::ofstream outFile("DEBUGTEXT.LOG");
                        outFile << "NOTE: This is not shared with anyone, if you have issues this could help if something is wrong" << std::endl;
                        outFile << "Computer Name: EXAMPLE_COMPUTER_NAME" << std::endl;
                        outFile << "CPU Model: EXAMPLE_CPU_MODEL" << std::endl;
                        outFile << "RAM Info: EXAMPLE_RAM_INFO" << std::endl;
                        outFile << "Storage Info: EXAMPLE_STORAGE_INFO" << std::endl;
                        outFile << "CPU usage history: [";
                        for (auto val : cpuHistory) outFile << val << ", ";
                        outFile << "]" << std::endl;
                        outFile << "RAM usage history: [";
                        for (auto val : ramHistory) outFile << val << ", ";
                        outFile << "]" << std::endl;
                        outFile.close();
                        std::cout << "File created and written successfully." << std::endl;
                    }
                    ImGui::EndTabItem();
                }
            }

            if (ImGui::BeginTabItem("Documentation")) {
                ImGui::Text("This is the link to the documentation below!");
                if (ImGui::Button("[Link] Editor Documentation")) {
                    system("start https://k754a.github.io/Editor%20Documentation");
                }
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }
}